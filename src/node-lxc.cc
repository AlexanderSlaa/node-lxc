//
// Created by alexander-slaa on 11/23/23.
//

#include <node/node_api.h>
#include <napi.h>

#include "LXC/LXC.h"

using namespace Napi;

//region static function mappings

Object version_object(Env env) {
    auto versionObject = Object::New(env);
    versionObject.Set("devel", Number::New(env, LXC_DEVEL));
    versionObject.Set("major", Number::New(env, LXC_VERSION_MAJOR));
    versionObject.Set("minor", Number::New(env, LXC_VERSION_MINOR));
    versionObject.Set("micro", Number::New(env, LXC_VERSION_MICRO));
    versionObject.Set("abi", String::New(env, LXC_VERSION_ABI));
    return versionObject;
}

Value get_global_config_item(const CallbackInfo &info) {
    if (info[0].IsString()) {
        return String::New(info.Env(), LXC::get_global_config_item(info[0].ToString().Utf8Value().c_str()));
    }
    return info.Env().Null();
}

Value _list_all_containers(const CallbackInfo &info) {
    Env env = info.Env();
    // You can specify the LXC path or set it to NULL for the default path
    char **names;
    // Get the list of all containers
    int num_containers = LXC::_list_all_containers(&names, nullptr,
                                                   info[0].IsString() ? info[0].ToString().Utf8Value().c_str()
                                                                      : nullptr);
    if (num_containers == -1) {
        Error::New(env, strerror(errno)).ThrowAsJavaScriptException();
        return env.Null();
    }
    // Create an array to store container names
    Array result = Array::New(env);
    // Iterate through the containers and add their names to the array
    for (int i = 0; i < num_containers; ++i) {
        const char *name = names[i];
        result.Set(i, String::New(env, name));
    }
    return result;
}

Value _list_defined_containers(const CallbackInfo &info) {
    Env env = info.Env();
    // You can specify the LXC path or set it to NULL for the default path
    char **names;
    // Get the list of all containers
    int num_containers = LXC::_list_defined_containers(&names, nullptr,
                                                       info[0].IsString() ? info[0].ToString().Utf8Value().c_str()
                                                                          : nullptr);
    if (num_containers == -1) {
        Error::New(env, strerror(errno)).ThrowAsJavaScriptException();
        return env.Null();
    }
    // Create an array to store container names
    Array result = Array::New(env);
    // Iterate through the containers and add their names to the array
    for (int i = 0; i < num_containers; ++i) {
        const char *name = names[i];
        result.Set(i, String::New(env, name));
    }
    return result;
}

Value _list_active_containers(const CallbackInfo &info) {
    Env env = info.Env();
    char **names;
    // Get the list of all containers
    int num_containers = LXC::_list_active_containers(&names, nullptr,
                                                      info[0].IsString() ? info[0].ToString().Utf8Value().c_str()
                                                                         : nullptr);
    if (num_containers == -1) {
        Error::New(env, strerror(errno)).ThrowAsJavaScriptException();
        return env.Null();
    }
    // Create an array to store container names
    Array result = Array::New(env);
    // Iterate through the containers and add their names to the array
    for (int i = 0; i < num_containers; ++i) {
        const char *name = names[i];
        result.Set(i, String::New(env, name));
    }
    return result;
}

Value _lxc_has_api_extension(const CallbackInfo &info) {
    if (!info[0].IsString()) {
        return Boolean::New(info.Env(), false);
    }
    return Boolean::New(info.Env(), LXC::_lxc_has_api_extension(info[0].ToString().Utf8Value().c_str()));
}

Value config_item_is_supported(const CallbackInfo &info) {
    if (!info[0].IsString()) {
        return Boolean::New(info.Env(), false);
    }
    return Boolean::New(info.Env(), LXC::config_item_is_supported(info[0].ToString().Utf8Value().c_str()));
}

Value _lxc_container_new(const CallbackInfo &info) {
    Env env = info.Env();
    // Return a unique identifier or handle to the container
    return External<LXC::Container>::New(env, LXC::Container::New(
            env,
            info[0].IsString() ? info[0].ToString().Utf8Value().c_str() : nullptr,
            info[1].IsString() ? info[1].ToString().Utf8Value().c_str() : nullptr
    ));
}

//endregion

//region container


Value lxc_defined(const CallbackInfo &info) {
    if (!info[0].IsExternal()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return Boolean::New(info.Env(), ref.Data()->is_defined());
}

Value lxc_state(const CallbackInfo &info) {
    if (!info[0].IsExternal()) {
        TypeError::New(info.Env(), "Invalid parameters").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return String::New(info.Env(), ref.Data()->state());
}

Value lxc_running(const CallbackInfo &info) {
    if (!info[0].IsExternal()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return Boolean::New(info.Env(), ref.Data()->is_running());
}

Value lxc_freeze(const CallbackInfo &info) {
    if (!info[0].IsExternal()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return Boolean::New(info.Env(), ref.Data()->freeze());
}

Value lxc_unfreeze(const CallbackInfo &info) {
    if (!info[0].IsExternal()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return Boolean::New(info.Env(), ref.Data()->unfreeze());
}

Value lxc_init_pid(const CallbackInfo &info) {
    if (!info[0].IsExternal()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return Number::New(info.Env(), ref.Data()->init_pid());
}

Value lxc_init_pidfd(const CallbackInfo &info) {
    if (!info[0].IsExternal()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return Number::New(info.Env(), ref.Data()->init_pidfd());
}

Value lxc_seccomp_notify_fd(const CallbackInfo &info) {
    if (!info[0].IsExternal()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return Number::New(info.Env(), ref.Data()->seccomp_notify_fd());
}

Value lxc_seccomp_notify_fd_active(const CallbackInfo &info) {
    if (!info[0].IsExternal()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return Number::New(info.Env(), ref.Data()->seccomp_notify_fd_active());
}


Value lxc_set_timeout(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsNumber()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return Number::New(info.Env(), ref.Data()->set_timeout(info[0].ToNumber().Int32Value()));
}


Value lxc_devpts_fd(const CallbackInfo &info) {
    if (!info[0].IsExternal()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return Number::New(info.Env(), ref.Data()->devpts_fd());
}

Value lxc_want_daemonize(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsBoolean()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return Boolean::New(info.Env(), ref.Data()->want_daemonize(info[1].ToBoolean().Value()));
}


Value lxc_want_close_all_fds(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsBoolean()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return Boolean::New(info.Env(), ref.Data()->want_close_all_fds(info[1].ToBoolean().Value()));
}

Value lxc_create(const CallbackInfo &info) {
    Env env = info.Env();

//    if (!info[0].IsExternal() || !info[1].IsString() || !info[2].IsString() || !info[3].IsObject() ||
//        !info[4].IsNumber() || !info[5].IsArray()) {
//        TypeError::New(env, "Invalid parameter").ThrowAsJavaScriptException();
//        return env.Null();
//    }

    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();

    // Parse container creation parameters
    std::string t = info[1].ToString().Utf8Value();
    std::string bdevtype = info[2].ToString().Utf8Value();

    //region bdev_options
    auto obj = info[3].ToObject();

    bdev_specs specs{
            .fstype = obj.Has("fstype") && obj.Get("fstype").IsString() ? (char *) obj.Get(
                    "fstype").ToString().Utf8Value().c_str() : nullptr,
            .fssize = obj.Has("fssize") && obj.Get("fstype").IsNumber() ? static_cast<uint64_t>(obj.Get(
                    "fssize").ToNumber().Int64Value()) : 0,
            .zfs{
                    .zfsroot = obj.Has("zfs") && obj.Get("zfs").IsObject() && obj.Get("zfs").ToObject().Has("zfsroot")
                               ? (char *) obj.Get("zfs").ToObject().Get("zfsroot").ToString().Utf8Value().c_str()
                               : nullptr,
            },
            .lvm{
                    .vg = obj.Has("lvm") && obj.Get("lvm").IsObject() && obj.Get("lvm").ToObject().Has("vg")
                          ? (char *) obj.Get("lvm").ToObject().Get("vg").ToString().Utf8Value().c_str() : nullptr,
                    .lv =obj.Has("lvm") && obj.Get("lvm").IsObject() && obj.Get("lvm").ToObject().Has("lv")
                         ? (char *) obj.Get("lvm").ToObject().Get("lv").ToString().Utf8Value().c_str() : nullptr,
                    .thinpool = obj.Has("lvm") && obj.Get("lvm").IsObject() && obj.Get("lvm").ToObject().Has("thinpool")
                                ? (char *) obj.Get("lvm").ToObject().Get("thinpool").ToString().Utf8Value().c_str()
                                : nullptr
            },
            .dir = obj.Has("dir") && obj.Get("dir").IsString() ? (char *) obj.Get("dir").ToString().Utf8Value().c_str()
                                                               : nullptr,
            .rbd{
                    .rbdname= obj.Has("rbd") && obj.Get("rbd").IsObject() && obj.Get("rbd").ToObject().Has("rbdname")
                              ? (char *) obj.Get("rdb").ToObject().Get("rbdname").ToString().Utf8Value().c_str()
                              : nullptr,
                    .rbdpool=obj.Has("rbd") && obj.Get("rbd").IsObject() && obj.Get("rbd").ToObject().Has("rbdpool")
                             ? (char *) obj.Get("rdb").ToObject().Get("rbdpool").ToString().Utf8Value().c_str()
                             : nullptr,
            },
    };
    //endregion

    int flags = info[4].ToNumber().Int32Value();


    // Convert JavaScript array to char* const argv[]
    Array argvArray = info[5].As<Array>();
    size_t argc = argvArray.Length();
    printf("arrayLen: %zu\n", argc);
    char **argv = new char *[argc + 1];
    for (size_t i = 0; i < argc; ++i) {
        argv[i] = strdup(argvArray.Get(i).ToString().Utf8Value().c_str());
    }
    argv[argc] = nullptr;
    argc++;

    // Call the C++ implementation
    bool result = ref.Data()->create(t.c_str(), bdevtype.c_str(), &specs, flags, argv, argc);

    // Free allocated memory
    for (size_t i = 0; i < argc; ++i) {
        free(const_cast<char *>(argv[i]));
    }
    delete[] argv;

    return Boolean::New(env, result);
}


Value lxc_start(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsNumber() && !info[2].IsArray()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();

    char **argv = nullptr;
    size_t argc;
    if (info[2].IsArray()) {     // Convert JavaScript array to char* const argv[]
        Array argvArray = info[2].As<Array>();
        argc = argvArray.Length();
        argv = new char *[argc + 1];
        for (size_t i = 0; i < argc; ++i) {
            argv[i] = strdup(argvArray.Get(i).ToString().Utf8Value().c_str());
        }
        argv[argc] = nullptr;
    }
    // Call the C++ implementation
    bool result = ref.Data()->start(info[1].ToNumber().Int32Value(), argv);

    if (argv != nullptr) {
        // Free allocated memory
        for (size_t i = 0; i < argc; ++i) {
            free(const_cast<char *>(argv[i]));
        }
        delete[] argv;
    }
    return Boolean::New(info.Env(), result);
}


Value lxc_stop(const CallbackInfo &info) {
    if (!info[0].IsExternal()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return Boolean::New(info.Env(), ref.Data()->stop());
}


Value lxc_reboot(const CallbackInfo &info) {
    if (!info[0].IsExternal()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return Boolean::New(info.Env(), ref.Data()->reboot());
}

Value lxc_shutdown(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsNumber()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return Boolean::New(info.Env(), ref.Data()->shutdown(info[1].ToNumber().Int32Value()));
}

Value lxc_config_file_name(const CallbackInfo &info) {
    if (!info[0].IsExternal()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    auto content = ref.Data()->config_file_name();
    auto res = String::New(info.Env(), content);
    free(content);
    return res;
}

Value lxc_destroy(const CallbackInfo &info) {
    if (!info[0].IsExternal()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return Boolean::New(info.Env(), ref.Data()->destroy());
}

Value lxc_destroy_with_snapshots(const CallbackInfo &info) {
    if (!info[0].IsExternal()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return Boolean::New(info.Env(), ref.Data()->destroy_with_snapshots());
}

Value lxc_wait(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsString() && !info[2].IsNumber()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return Boolean::New(info.Env(),
                        ref.Data()->wait(info[1].ToString().Utf8Value().c_str(),
                                         info[2].ToNumber().Int32Value())
    );
}

Value lxc_get_config_item(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsString()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    auto configItem = ref.Data()->get_config_item(info[1].ToString().Utf8Value().c_str());
    auto res = String::New(info.Env(), configItem);
    free(configItem);
    return res;
}

Value lxc_set_config_item(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsString() && !info[2].IsString()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return Boolean::New(info.Env(),
                        ref.Data()->set_config_item(

                                info[1].ToString().Utf8Value().c_str(),
                                info[2].ToString().Utf8Value().c_str()
                        )
    );
}

void lxc_clear_config(const CallbackInfo &info) {
    if (!info[0].IsExternal()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    ref.Data()->clear_config();
}

Boolean lxc_clear_config_item(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsString()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return Boolean::New(info.Env(),
                        ref.Data()->clear_config_item(info[1].ToString().Utf8Value().c_str()));
}


Boolean lxc_get_running_config_item(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsString()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return Boolean::New(info.Env(),
                        ref.Data()->get_running_config_item(
                                info[1].ToString().Utf8Value().c_str()));
}

String lxc_get_keys(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsString()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return String::New(info.Env(),
                       ref.Data()->get_keys(
                               info[1].ToString().Utf8Value().c_str()));
}

String lxc_get_cgroup_item(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsString()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return String::New(info.Env(),
                       ref.Data()->get_cgroup_item(
                               info[1].ToString().Utf8Value().c_str()));
}

Boolean lxc_set_cgroup_item(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsString() && !info[2].IsString()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return Boolean::New(info.Env(),
                        ref.Data()->set_cgroup_item(
                                info[1].ToString().Utf8Value().c_str(),
                                info[2].ToString().Utf8Value().c_str()));
}


String lxc_get_config_path(const CallbackInfo &info) {
    if (!info[0].IsExternal()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return String::New(info.Env(),
                       ref.Data()->get_config_path());
}


Boolean lxc_set_config_path(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsString()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return Boolean::New(info.Env(),
                        ref.Data()->set_config_path(
                                info[1].ToString().Utf8Value().c_str()));
}


Boolean lxc_load_config(const CallbackInfo &info) {
    if (!info[0].IsExternal()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return Boolean::New(info.Env(),
                        ref.Data()->load_config(
                                !info[1].IsString() ? info[1].ToString().Utf8Value().c_str() : nullptr));
}

Boolean lxc_save_config(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsString()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return Boolean::New(info.Env(),
                        ref.Data()->save_config(
                                info[1].ToString().Utf8Value().c_str()));
}


Boolean lxc_clone(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsString() && !info[2].IsString() && !info[3].IsNumber() &&
        !info[4].IsString()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return Boolean::New(info.Env(),
                        ref.Data()->clone(
                                info[1].ToString().Utf8Value().c_str(),
                                info[2].ToString().Utf8Value().c_str(),
                                info[3].ToNumber().Int32Value(),
                                info[4].ToString().Utf8Value().c_str()
                        )
    );
}

Number lxc_console_getfd(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsNumber()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return Number::New(info.Env(), ref.Data()->console_getfd(info[1].ToNumber().Int32Value()));
}


Boolean lxc_console(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsNumber() && !info[2].IsNumber() && !info[3].IsNumber() &&
        !info[4].IsNumber() &&
        !info[5].IsNumber()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return Boolean::New(info.Env(),
                        ref.Data()->console(
                                info[1].ToNumber().Int32Value(),
                                info[2].ToNumber().Int32Value(),
                                info[3].ToNumber().Int32Value(),
                                info[4].ToNumber().Int32Value(),
                                info[5].ToNumber().Int32Value()
                        )
    );
}

Array lxc_get_interfaces(const CallbackInfo &info) {
    if (!info[0].IsExternal()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();

    // Call the underlying C++ function
    char **interfaces = ref.Data()->get_interfaces();

    // Convert the result to a JavaScript array
    Napi::Array result = Napi::Array::New(info.Env());
    for (int i = 0; interfaces[i] != nullptr; ++i) {
        result.Set(i, Napi::String::New(info.Env(), interfaces[i]));
        // Remember to free the memory allocated by lxc
        free(interfaces[i]);
    }
    free(interfaces);
    return result;
}

Array lxc_get_ips(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsString() && !info[2].IsString() && !info[3].IsNumber()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();

    // Call the underlying C++ function
    char **ips = ref.Data()->get_ips(info[1].ToString().Utf8Value().c_str(),
                                     info[2].ToString().Utf8Value().c_str(), info[3].ToNumber().Int32Value());

    // Convert the result to a JavaScript array
    Napi::Array result = Napi::Array::New(info.Env());
    for (int i = 0; ips[i] != nullptr; ++i) {
        result.Set(i, Napi::String::New(info.Env(), ips[i]));
        // Remember to free the memory allocated by lxc
        free(ips[i]);
    }
    free(ips);
    return result;
}


Value lxc_add_device_node(const CallbackInfo &info) {
    if (!info[0].IsExternal() || !info[1].IsString() || !info[2].IsString()) {
        TypeError::New(info.Env(), "Invalid parameters").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return Boolean::New(info.Env(), ref.Data()->add_device_node(
            info[0].IsString() ? info[0].ToString().Utf8Value().c_str() : nullptr,
            info[1].IsString() ? info[1].ToString().Utf8Value().c_str() : nullptr
    ));
}


Value lxc_remove_device_node(const CallbackInfo &info) {
    if (!info[0].IsExternal() || !info[1].IsString() || !info[2].IsString()) {
        TypeError::New(info.Env(), "Invalid parameters").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return Boolean::New(info.Env(), ref.Data()->remove_device_node(
            info[0].IsString() ? info[0].ToString().Utf8Value().c_str() : nullptr,
            info[1].IsString() ? info[1].ToString().Utf8Value().c_str() : nullptr
    ));
}


Value lxc_rename(const CallbackInfo &info) {
    if (!info[0].IsExternal() || !info[1].IsString()) {
        TypeError::New(info.Env(), "Invalid parameters").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return Boolean::New(info.Env(), ref.Data()->rename(info[1].ToString().Utf8Value().c_str()));
}


Value lxc_checkpoint(const CallbackInfo &info) {
    if (!info[0].IsExternal() || !info[1].IsString() || !info[2].IsBoolean() || !info[3].IsBoolean()) {
        TypeError::New(info.Env(), "Invalid parameters").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return Boolean::New(info.Env(),
                        ref.Data()->checkpoint(
                                (char *) info[1].ToString().Utf8Value().c_str(),
                                info[2].ToBoolean(),
                                info[3].ToBoolean())
    );
}

Value lxc_restore(const CallbackInfo &info) {
    if (!info[0].IsExternal() || !info[1].IsString() || !info[2].IsBoolean() || !info[3].IsBoolean()) {
        TypeError::New(info.Env(), "Invalid parameters").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return Boolean::New(info.Env(),
                        ref.Data()->restore(
                                (char *) info[1].ToString().Utf8Value().c_str(),
                                info[2].ToBoolean())
    );
}


Value lxc_migrate(const CallbackInfo &info) {
    if (!info[0].IsExternal() || !info[1].IsNumber() || !info[2].IsObject()) {
        TypeError::New(info.Env(), "Invalid parameters").ThrowAsJavaScriptException();
        return info.Env().Null();
    }

    auto optsObj = info[2].ToObject();
    struct migrate_opts opts = {
            .directory = (char *) optsObj.Get("directory").ToString().Utf8Value().c_str(),
            .verbose = optsObj.Has("verbose") && optsObj.Get("verbose").ToBoolean(),
            .stop = optsObj.Has("stop") && optsObj.Get("stop").ToBoolean(),
            .predump_dir = optsObj.Has("predump_dir") ? (char *) optsObj.Get(
                    "predump_dir").ToString().Utf8Value().c_str() : nullptr,
            .pageserver_address = optsObj.Has("pageserver_address") ? (char *) optsObj.Get(
                    "pageserver_address").ToString().Utf8Value().c_str() : nullptr,
            .pageserver_port = optsObj.Has("pageserver_port") ? (char *) optsObj.Get(
                    "pageserver_port").ToString().Utf8Value().c_str() : nullptr,
#if VERSION_AT_LEAST(2, 0, 1)
            .preserves_inodes = optsObj.Has("preserves_inodes") && optsObj.Get("preserves_inodes").ToBoolean(),
#endif
#if VERSION_AT_LEAST(2, 0, 4)
            .action_script = optsObj.Has("action_script") ? (char *) optsObj.Get(
                    "action_script").ToString().Utf8Value().c_str() : nullptr,
            .disable_skip_in_flight = optsObj.Has("disable_skip_in_flight") && optsObj.Get(
                    "disable_skip_in_flight").ToBoolean(),
#endif
            .ghost_limit = optsObj.Has("ghost_limit") ? optsObj.Get("ghost_limit").ToNumber().Uint32Value() : 0,
#if VERSION_AT_LEAST(3, 0, 0)
            .features_to_check = optsObj.Has("features_to_check") ? optsObj.Get(
                    "features_to_check").ToNumber().Uint32Value() : 0,
#endif
    };
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return Number::New(info.Env(),
                       ref.Data()->migrate(info[0].ToNumber().Int32Value(), &opts));
}


Value lxc_attach_interface(const CallbackInfo &info) {
    if (!info[0].IsExternal() || !info[1].IsString() || !info[2].IsString()) {
        TypeError::New(info.Env(), "Invalid parameters").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return Boolean::New(info.Env(), ref.Data()->attach_interface(
            info[0].IsString() ? info[0].ToString().Utf8Value().c_str() : nullptr,
            info[1].IsString() ? info[1].ToString().Utf8Value().c_str() : nullptr
    ));
}


Value lxc_detach_interface(const CallbackInfo &info) {
    if (!info[0].IsExternal() || !info[1].IsString() || !info[2].IsString()) {
        TypeError::New(info.Env(), "Invalid parameters").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return Boolean::New(info.Env(), ref.Data()->detach_interface(
            info[0].IsString() ? info[0].ToString().Utf8Value().c_str() : nullptr,
            info[1].IsString() ? info[1].ToString().Utf8Value().c_str() : nullptr
    ));
}

Value lxc_error_num(const CallbackInfo &info) {
    if (!info[0].IsExternal()) {
        TypeError::New(info.Env(), "Invalid parameters").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    return Number::New(info.Env(), ref.Data()->error_num());
}


Value lxc_error_str(const CallbackInfo &info) {
    if (!info[0].IsExternal()) {
        TypeError::New(info.Env(), "Invalid parameters").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
    auto err_str = ref.Data()->error_str();
    if (err_str == nullptr) {
        return info.Env().Null();
    }
    return String::New(info.Env(), err_str);
}

//Value lxc_console_log(const CallbackInfo &info) {
//    if (!info[0].IsExternal()) {
//        TypeError::New(info.Env(), "Invalid parameters").ThrowAsJavaScriptException();
//        return info.Env().Null();
//    }
//    // Get the container handle from JavaScript
//    External<LXC::Container> ref = info[0].As<External<LXC::Container>>();
//    return Number::New(info.Env(), ref.Data()->error_num());
//}

//endregion

Object Init(Env env, Object exports) {
    //region general
    exports.Set(String::New(env, "version"), version_object(env));
    exports.Set(String::New(env, "get_global_config_item"), Function::New(env, get_global_config_item));
    exports.Set(String::New(env, "list_all_containers"), Function::New(env, _list_all_containers));
    exports.Set(String::New(env, "list_defined_containers"), Function::New(env, _list_defined_containers));
    exports.Set(String::New(env, "list_active_containers"), Function::New(env, _list_active_containers));
    exports.Set(String::New(env, "lxc_container_new"), Function::New(env, _lxc_container_new));
    exports.Set(String::New(env, "lxc_has_api_extension"), Function::New(env, _lxc_has_api_extension));
    exports.Set(String::New(env, "lxc_config_item_is_supported"), Function::New(env, config_item_is_supported));
    //endregion
    //region container
    exports.Set(String::New(env, "lxc_defined"), Function::New(env, lxc_defined));
    exports.Set(String::New(env, "lxc_state"), Function::New(env, lxc_state));
    exports.Set(String::New(env, "lxc_running"), Function::New(env, lxc_running));
    exports.Set(String::New(env, "lxc_freeze"), Function::New(env, lxc_freeze));
    exports.Set(String::New(env, "lxc_unfreeze"), Function::New(env, lxc_unfreeze));
    exports.Set(String::New(env, "lxc_init_pid"), Function::New(env, lxc_init_pid));
    exports.Set(String::New(env, "lxc_init_pidfd"), Function::New(env, lxc_init_pidfd));
    exports.Set(String::New(env, "lxc_seccomp_notify_fd"), Function::New(env, lxc_seccomp_notify_fd));
    exports.Set(String::New(env, "lxc_seccomp_notify_fd_active"), Function::New(env, lxc_seccomp_notify_fd_active));
    exports.Set(String::New(env, "lxc_set_timeout"), Function::New(env, lxc_set_timeout));
    exports.Set(String::New(env, "lxc_devpts_fd"), Function::New(env, lxc_devpts_fd));
    exports.Set(String::New(env, "lxc_want_daemonize"), Function::New(env, lxc_want_daemonize));
    exports.Set(String::New(env, "lxc_want_close_all_fds"), Function::New(env, lxc_want_close_all_fds));
    exports.Set(String::New(env, "lxc_create"), Function::New(env, lxc_create));
    exports.Set(String::New(env, "lxc_start"), Function::New(env, lxc_start));
    exports.Set(String::New(env, "lxc_stop"), Function::New(env, lxc_stop));
    exports.Set(String::New(env, "lxc_reboot"), Function::New(env, lxc_reboot));
    exports.Set(String::New(env, "lxc_shutdown"), Function::New(env, lxc_shutdown));
    exports.Set(String::New(env, "lxc_config_file_name"), Function::New(env, lxc_config_file_name));
    exports.Set(String::New(env, "lxc_destroy"), Function::New(env, lxc_destroy));
    exports.Set(String::New(env, "lxc_destroy_with_snapshots"), Function::New(env, lxc_destroy_with_snapshots));
    exports.Set(String::New(env, "lxc_wait"), Function::New(env, lxc_wait));
    exports.Set(String::New(env, "lxc_get_config_item"), Function::New(env, lxc_get_config_item));
    exports.Set(String::New(env, "lxc_set_config_item"), Function::New(env, lxc_set_config_item));
    exports.Set(String::New(env, "lxc_clear_config"), Function::New(env, lxc_clear_config));
    exports.Set(String::New(env, "lxc_clear_config_item"), Function::New(env, lxc_clear_config_item));
    exports.Set(String::New(env, "lxc_get_running_config_item"), Function::New(env, lxc_get_running_config_item));
    exports.Set(String::New(env, "lxc_get_keys"), Function::New(env, lxc_get_keys));
    exports.Set(String::New(env, "lxc_get_cgroup_item"), Function::New(env, lxc_get_cgroup_item));
    exports.Set(String::New(env, "lxc_set_cgroup_item"), Function::New(env, lxc_set_cgroup_item));
    exports.Set(String::New(env, "lxc_get_config_path"), Function::New(env, lxc_get_config_path));
    exports.Set(String::New(env, "lxc_set_config_path"), Function::New(env, lxc_set_config_path));
    exports.Set(String::New(env, "lxc_load_config"), Function::New(env, lxc_load_config));
    exports.Set(String::New(env, "lxc_save_config"), Function::New(env, lxc_save_config));
    exports.Set(String::New(env, "lxc_clone"), Function::New(env, lxc_clone));
    exports.Set(String::New(env, "lxc_console_getfd"), Function::New(env, lxc_console_getfd));
    exports.Set(String::New(env, "lxc_console"), Function::New(env, lxc_console));
    exports.Set(String::New(env, "lxc_get_interfaces"), Function::New(env, lxc_get_interfaces));
    exports.Set(String::New(env, "lxc_get_ips"), Function::New(env, lxc_get_ips));


    exports.Set(String::New(env, "lxc_add_device_node"), Function::New(env, lxc_add_device_node));
    exports.Set(String::New(env, "lxc_remove_device_node"), Function::New(env, lxc_remove_device_node));
    exports.Set(String::New(env, "lxc_rename"), Function::New(env, lxc_rename));
    exports.Set(String::New(env, "lxc_checkpoint"), Function::New(env, lxc_checkpoint));
    exports.Set(String::New(env, "lxc_restore"), Function::New(env, lxc_restore));
    exports.Set(String::New(env, "lxc_migrate"), Function::New(env, lxc_migrate));
    exports.Set(String::New(env, "lxc_attach_interface"), Function::New(env, lxc_attach_interface));
    exports.Set(String::New(env, "lxc_detach_interface"), Function::New(env, lxc_detach_interface));
    exports.Set(String::New(env, "lxc_detach_interface"), Function::New(env, lxc_detach_interface));
    exports.Set(String::New(env, "lxc_error_num"), Function::New(env, lxc_error_num));
    exports.Set(String::New(env, "lxc_error_str"), Function::New(env, lxc_error_str));

    //endregion
    return exports;
}


NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
