//
// Created by alexanderslaa on 12/16/23.
//

#include <sys/wait.h>
#include "Container.h"

Napi::Object Container::Init(Napi::Env env, Napi::Object exports)
{
    Napi::Function func =
        DefineClass(env, "Container", {
                                          InstanceAccessor("name", &Container::GetName, nullptr),
                                          InstanceAccessor("state", &Container::GetState, nullptr),
                                          InstanceMethod("start", &Container::Start),
                                          InstanceMethod("stop", &Container::Stop),
                                          InstanceMethod("create", &Container::Create),
                                          InstanceMethod("getConfigItem", &Container::GetConfigItem),
                                          InstanceMethod("setConfigItem", &Container::SetConfigItem),
                                          InstanceMethod("clearConfigItem", &Container::ClearConfigItem),
                                          InstanceMethod("clearConfig", &Container::ClearConfig),
                                          InstanceMethod("attach", &Container::Attach),
                                          InstanceMethod("exec", &Container::Exec),
                                          InstanceMethod("daemonize", &Container::Daemonize),
                                      });

    auto constructor = Napi::Persistent(func);
    constructor.SuppressDestruct(); // Prevent the destructor, as it will be handled by N-API.
    exports.Set("Container", func);
    return exports;
}

Napi::Value Container::GetName(const Napi::CallbackInfo &info)
{
    return Napi::String::New(info.Env(), _container->name);
}

Napi::Value Container::GetState(const Napi::CallbackInfo &info)
{
    return Napi::String::New(info.Env(), _container->state(_container));
}

Container::Container(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Container>(info)
{
    Napi::Env env = info.Env();
    if (info.Length() <= 0 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
        return;
    }
    _container = lxc_container_new(info[0].ToString().Utf8Value().c_str(),
                                   info[1].IsString() ? info[1].ToString().Utf8Value().c_str() : lxc_get_global_config_item("lxc.lxcpath"));
    if (info[2].IsString() && strcmp(info[2].ToString().Utf8Value().c_str(), "none") != 0)
    {
        _container->load_config(_container, info[2].IsString() ? info[2].ToString().Utf8Value().c_str() : lxc_get_global_config_item("lxc.default_config"));
    }
}

Container::~Container()
{
    if (_container)
    {
        // Clean up the container if needed
        lxc_container_put(_container);
    }
}

Napi::Value Container::Start(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    if (info.Length() <= 0 || !info[0].IsNumber() || !info[1].IsArray())
    {
        Napi::TypeError::New(env, "Invalid arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    // Get the array of strings from the JavaScript side
    Napi::Array jsArray = info[1].As<Napi::Array>();

    // Allocate memory for char* pointers
    char **argv = new char *[jsArray.Length() + 1]; // +1 for the null terminator

    // Copy the strings to the allocated memory
    for (size_t i = 0; i < jsArray.Length(); ++i)
    {
        Napi::Value element = jsArray.Get(i);
        if (!element.IsString())
        {
            Napi::TypeError::New(env, "String expected in the array").ThrowAsJavaScriptException();
            delete[] argv; // Cleanup on error
            return env.Null();
        }
        std::string str = element.As<Napi::String>().Utf8Value();
        argv[i] = strdup(str.c_str());
    }

    // Null-terminate the char* array
    argv[jsArray.Length()] = NULL;

    // char *const default_args[] = {
    //     ,
    //     NULL,
    // };

    if (!_container->may_control(_container))
    {
        Napi::Error::New(env, "Insufficent privileges to control container").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (_container->is_running(_container))
    {
        return Napi::Boolean::New(env, true);
    }

    if (!_container->start(_container, info[0].ToNumber().Int32Value(), argv))
    {
        printf("The container failed to start");
        Napi::Error::New(env, strerror(_container->error_num)).ThrowAsJavaScriptException();
        return env.Null();
    }

    // Cleanup: Free allocated memory
    for (size_t i = 0; i < jsArray.Length(); ++i)
    {
        delete[] argv[i];
    }
    delete[] argv;

    return Napi::Boolean::New(env, true);
}

Napi::Value Container::Stop(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    // Call the stop function
    auto res = _container->stop(_container);
    return Napi::Boolean::New(env, res);
}

Napi::Value Container::Create(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (_container->is_defined(_container))
    {
        Napi::Error::New(env, "Container already exits").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (info.Length() <= 4 || !info[0].IsString() || !info[1].IsString() || !info[2].IsObject() ||
        !info[3].IsNumber() || !info[4].IsArray())
    {
        Napi::TypeError::New(env, "Invalid arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    // Parse container creation parameters
    char *t = info[0].IsString() ? strdup(info[0].ToString().Utf8Value().c_str()) : nullptr;
    char *bdevtype = info[1].IsString() ? strdup(info[1].ToString().Utf8Value().c_str()) : nullptr;

    // Handle struct bdev_specs (assuming it's an object with specific properties)
    Napi::Object bdevSpecsObj = info[2].As<Napi::Object>();
    // Extract values from bdevSpecsObj and populate your struct bdev_specs
    bdev_specs bdevSpecs{
        .fstype = bdevSpecsObj.Has("fstype") && bdevSpecsObj.Get("fstype").IsString() ? strdup(bdevSpecsObj.Get(
                                                                                                               "fstype")
                                                                                                   .ToString()
                                                                                                   .Utf8Value()
                                                                                                   .c_str())
                                                                                      : nullptr,
        .fssize = bdevSpecsObj.Has("fssize") && bdevSpecsObj.Get("fstype").IsNumber()
                      ? static_cast<uint64_t>(bdevSpecsObj.Get(
                                                              "fssize")
                                                  .ToNumber()
                                                  .Int64Value())
                      : 0,
        .zfs{
            .zfsroot = bdevSpecsObj.Has("zfs") && bdevSpecsObj.Get("zfs").IsObject() &&
                               bdevSpecsObj.Get("zfs").ToObject().Has("zfsroot")
                           ? (char *)strdup(
                                 bdevSpecsObj.Get("zfs").ToObject().Get("zfsroot").ToString().Utf8Value().c_str())
                           : nullptr,
        },
        .lvm{
            .vg = bdevSpecsObj.Has("lvm") && bdevSpecsObj.Get("lvm").IsObject() &&
                          bdevSpecsObj.Get("lvm").ToObject().Has("vg")
                      ? (char *)strdup(bdevSpecsObj.Get("lvm").ToObject().Get("vg").ToString().Utf8Value().c_str())
                      : nullptr,
            .lv = bdevSpecsObj.Has("lvm") && bdevSpecsObj.Get("lvm").IsObject() &&
                          bdevSpecsObj.Get("lvm").ToObject().Has("lv")
                      ? (char *)strdup(bdevSpecsObj.Get("lvm").ToObject().Get("lv").ToString().Utf8Value().c_str())
                      : nullptr,
            .thinpool = bdevSpecsObj.Has("lvm") && bdevSpecsObj.Get("lvm").IsObject() &&
                                bdevSpecsObj.Get("lvm").ToObject().Has("thinpool")
                            ? (char *)strdup(
                                  bdevSpecsObj.Get("lvm").ToObject().Get("thinpool").ToString().Utf8Value().c_str())
                            : nullptr},
        .dir = bdevSpecsObj.Has("dir") && bdevSpecsObj.Get("dir").IsString() ? (char *)strdup(
                                                                                   bdevSpecsObj.Get("dir").ToString().Utf8Value().c_str())
                                                                             : nullptr,
        .rbd{
            .rbdname = bdevSpecsObj.Has("rbd") && bdevSpecsObj.Get("rbd").IsObject() &&
                               bdevSpecsObj.Get("rbd").ToObject().Has("rbdname")
                           ? (char *)strdup(
                                 bdevSpecsObj.Get("rdb").ToObject().Get("rbdname").ToString().Utf8Value().c_str())
                           : nullptr,
            .rbdpool = bdevSpecsObj.Has("rbd") && bdevSpecsObj.Get("rbd").IsObject() &&
                               bdevSpecsObj.Get("rbd").ToObject().Has("rbdpool")
                           ? (char *)strdup(
                                 bdevSpecsObj.Get("rdb").ToObject().Get("rbdpool").ToString().Utf8Value().c_str())
                           : nullptr,
        },
    };
    // Get the creation flags for the container
    int flags = info[3].ToNumber().Int32Value();

    // Get the array of strings from the JavaScript side
    Napi::Array jsArray = info[4].As<Napi::Array>();

    // Allocate memory for char* pointers
    char **argv = new char *[jsArray.Length() + 1]; // +1 for the null terminator

    // Copy the strings to the allocated memory
    for (size_t i = 0; i < jsArray.Length(); ++i)
    {
        Napi::Value element = jsArray.Get(i);
        if (!element.IsString())
        {
            Napi::TypeError::New(env, "String expected in the array").ThrowAsJavaScriptException();
            delete[] argv; // Cleanup on error
            return env.Null();
        }

        std::string str = element.As<Napi::String>().Utf8Value();
        argv[i] = new char[str.length() + 1]; // +1 for the null terminator
        strcpy(argv[i], str.c_str());
    }

    // Null-terminate the char* array
    argv[jsArray.Length()] = nullptr;

    // Call the create function
    auto res = _container->create(_container, t, bdevtype, &bdevSpecs, flags, argv);
    if (!res)
    {
        Napi::Error::New(env, strerror(errno)).ThrowAsJavaScriptException();
        delete[] argv; // Cleanup on error
        return env.Null();
    }

    // Cleanup: Free allocated memory
    for (size_t i = 0; i < jsArray.Length(); ++i)
    {
        delete[] argv[i];
    }
    delete[] argv;
    free(t);
    free(bdevtype);

    return Napi::Boolean::New(env, res);
}

Napi::Value Container::GetConfigItem(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    if (info.Length() <= 0 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
        return env.Null();
    }

    char *value;

    int len = _container->get_config_item(_container, info[0].ToString().Utf8Value().c_str(), nullptr, 0);
    if (len <= 0)
    {
        return env.Null();
    }

again:
    value = (char *)malloc(sizeof(char) * len + 1);
    if (value == nullptr)
        goto again;

    if (_container->get_config_item(_container, info[0].ToString().Utf8Value().c_str(), value, len + 1) != len)
    {
        free(value);
        return env.Null();
    }

    return Napi::String::New(env, value);
}

Napi::Value Container::SetConfigItem(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    if (info.Length() <= 0 || !info[0].IsString() || !info[1].IsString())
    {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
        return env.Null();
    }
    return Napi::Boolean::New(env, _container->set_config_item(_container, info[0].ToString().Utf8Value().c_str(),
                                                               info[1].ToString().Utf8Value().c_str()));
}

Napi::Value Container::ClearConfigItem(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    if (info.Length() <= 0 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
        return env.Null();
    }
    return Napi::Boolean::New(env, _container->clear_config_item(_container, info[0].ToString().Utf8Value().c_str()));
}

void Container::ClearConfig(const Napi::CallbackInfo &info)
{
    _container->clear_config(_container);
}

int wait_for_pid_status(pid_t pid)
{
    int status, ret;

again:
    ret = waitpid(pid, &status, 0);
    if (ret == -1)
    {
        if (errno == EINTR)
            goto again;
        return -1;
    }
    if (ret != pid)
        goto again;
    return status;
}

Napi::Value Container::Attach(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    if (info.Length() <= 0 ||
        !info[0].IsBoolean() || // CLEAN_ENV
        !info[1].IsNumber() ||  // NAMESPACE
        !info[2].IsNumber() ||  // PERSIONALITY
        !info[3].IsNumber() ||  // UID
        !info[4].IsNumber() ||  // GUID
        !info[5].IsArray() ||   // GROUPS
        !info[6].IsArray() ||   // STDIO
        !info[7].IsString() ||  // CWD
        !info[8].IsArray() ||   // ENV
        !info[9].IsArray() ||   // KEEP_ENV
        !info[10].IsNumber()    // FLAGS
    )
    {
        Napi::TypeError::New(env, "Invalid parameters").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!_container->is_running(_container))
    {
        Napi::Error::New(env, "Container is not running").ThrowAsJavaScriptException();
        return env.Null();
    }

    int ret;
    pid_t pid;

    lxc_attach_options_t attach_options = LXC_ATTACH_OPTIONS_DEFAULT;
    attach_options.attach_flags = info[10].ToNumber().Int32Value();

    attach_options.env_policy = LXC_ATTACH_KEEP_ENV;
    if (info[0].ToBoolean())
    {
        attach_options.env_policy = LXC_ATTACH_CLEAR_ENV;
    }

    attach_options.namespaces = info[1].ToNumber().Int32Value();
    attach_options.personality = info[2].ToNumber().Int32Value();

    attach_options.uid = info[3].ToNumber().Int32Value();
    attach_options.gid = info[4].ToNumber().Int32Value();
#if VERSION_AT_LEAST(4, 0, 9)
    Napi::Array groups_array = info[5].As<Napi::Array>();
    size_t groups_array_len = groups_array.Length();
    lxc_groups_t groups = {
        .size = groups_array_len,
        .list = groups_array_len > 0 ? new gid_t[groups_array_len] : nullptr};
    if (groups.list != nullptr)
    {
        for (size_t i = 0; i < groups_array_len; ++i)
        {
            groups_array[i] = strdup(groups_array.Get(i).ToString().Utf8Value().c_str());
        }
    }

    if (groups.size > 0)
    {
        attach_options.groups = groups;
        attach_options.attach_flags &= LXC_ATTACH_SETGROUPS;
    }
#endif

    attach_options.stdin_fd = info[6].As<Napi::Array>().Get((uint32_t)0).ToNumber().Int32Value(); // STDINFD
    attach_options.stdout_fd = info[6].As<Napi::Array>().Get((uint32_t)1).ToNumber().Int32Value();
    attach_options.stderr_fd = info[6].As<Napi::Array>().Get((uint32_t)2).ToNumber().Int32Value();

    attach_options.initial_cwd = strdup(info[7].ToString().Utf8Value().c_str());

    // Get the array of strings from the JavaScript side
    Napi::Array js_extra_env_vars = info[8].As<Napi::Array>();

    // Allocate memory for char* pointers
    char **extra_env_vars = new char *[js_extra_env_vars.Length() + 1]; // +1 for the null terminator

    // Copy the strings to the allocated memory
    for (size_t i = 0; i < js_extra_env_vars.Length(); ++i)
    {
        Napi::Value element = js_extra_env_vars.Get(i);
        if (!element.IsString())
        {
            Napi::TypeError::New(env, "String expected in the array").ThrowAsJavaScriptException();
            delete[] extra_env_vars; // Cleanup on error
            return env.Null();
        }

        std::string str = element.As<Napi::String>().Utf8Value();
        extra_env_vars[i] = new char[str.length() + 1]; // +1 for the null terminator
        strcpy(extra_env_vars[i], str.c_str());
    }

    // Null-terminate the char* array
    extra_env_vars[js_extra_env_vars.Length()] = nullptr;

    attach_options.extra_env_vars = extra_env_vars;

    // Get the array of strings from the JavaScript side
    Napi::Array js_extra_keep_env = info[9].As<Napi::Array>();

    // Allocate memory for char* pointers
    char **extra_keep_env = new char *[js_extra_keep_env.Length() + 1]; // +1 for the null terminator

    // Copy the strings to the allocated memory
    for (size_t i = 0; i < js_extra_keep_env.Length(); ++i)
    {
        Napi::Value element = js_extra_keep_env.Get(i);
        if (!element.IsString())
        {
            Napi::TypeError::New(env, "String expected in the array").ThrowAsJavaScriptException();
            delete[] extra_env_vars; // Cleanup on error
            return env.Null();
        }

        std::string str = element.As<Napi::String>().Utf8Value();
        extra_keep_env[i] = new char[str.length() + 1]; // +1 for the null terminator
        strcpy(extra_keep_env[i], str.c_str());
    }

    // Null-terminate the char* array
    extra_keep_env[js_extra_keep_env.Length()] = nullptr;

    attach_options.extra_keep_env = extra_keep_env;

    ret = _container->attach(_container, lxc_attach_run_shell, NULL, &attach_options, &pid);
    if (ret < 0)
    {
        goto end;
    }

    ret = wait_for_pid_status(pid);
    if (ret < 0)
    {
        goto end;
    }

    if (WIFEXITED(ret))
    {
        ret = WEXITSTATUS(ret);
        goto end;
    }
end:
    // free extra_env_vars
    for (size_t i = 0; i < js_extra_env_vars.Length(); ++i)
    {
        delete[] extra_env_vars[i];
    }
    delete[] extra_env_vars;

    // free extra_keep_env
    for (size_t i = 0; i < js_extra_keep_env.Length(); ++i)
    {
        delete[] extra_keep_env[i];
    }
    delete[] extra_keep_env;

    return Napi::Number::New(env, ret);
}

Napi::Value Container::Exec(const Napi::CallbackInfo &info)
{
    return Napi::Value();
}

Napi::Value Container::Daemonize(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    if (info.Length() <= 0 || !info[0].IsBoolean())
    {
        Napi::TypeError::New(env, "Invalid parameter").ThrowAsJavaScriptException();
        return env.Null();
    }
    return Napi::Boolean::New(env, _container->want_daemonize(_container, info[0].ToBoolean()));
}
