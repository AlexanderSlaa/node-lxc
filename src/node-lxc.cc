#include <node/node_api.h>
#include <napi.h>

#include <lxc/lxccontainer.h>
#include <lxc/version.h>

#include "bindings/LXCContainer.h"
#include "bindings/lxc.h"

using namespace Napi;

//region version macros

#define VERSION_AT_LEAST(major, minor, micro)                            \
    ((!(major > LXC_VERSION_MAJOR ||                    \
    major == LXC_VERSION_MAJOR && minor > LXC_VERSION_MINOR ||                \
    major == LXC_VERSION_MAJOR && minor == LXC_VERSION_MINOR && micro > LXC_VERSION_MICRO)))
//endregion

//region generic

String version(const CallbackInfo &info) {
    Env env = info.Env();
    return String::New(env, std::to_string(LXC_VERSION_MAJOR) + "." + std::to_string(LXC_VERSION_MINOR) + "." +
                            std::to_string(LXC_VERSION_MICRO));
}

Object version_object(const CallbackInfo &info) {
    Env env = info.Env();
    auto versionObject = Object::New(env);
    versionObject.Set("major", Number::New(env, LXC_VERSION_MAJOR));
    versionObject.Set("minor", Number::New(env, LXC_VERSION_MINOR));
    versionObject.Set("micro", Number::New(env, LXC_VERSION_MICRO));
    return versionObject;
}

Boolean is_devel(const CallbackInfo &info) {
    return Boolean::New(info.Env(), LXC_DEVEL);
}

Value _lxc_get_global_config_item(const CallbackInfo &info) {
    if (info[0].IsString()) {
        return String::New(info.Env(), lxc_get_global_config_item(info[0].ToString().Utf8Value().c_str()));
    }
    return info.Env().Null();
}

Value _list_all_containers(const CallbackInfo &info) {
    Env env = info.Env();
    // You can specify the LXC path or set it to NULL for the default path
    const char *lxcpath;
    if (!info[0].IsString()) {
        lxcpath = lxc_get_global_config_item("lxc.lxcpath");
    } else {
        lxcpath = info[0].ToString().Utf8Value().c_str();
    }

    char **names;
    // Get the list of all containers
    int num_containers = list_all_containers(lxcpath, &names, nullptr);
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
    const char *lxcpath;
    if (!info[0].IsString()) {
        lxcpath = lxc_get_global_config_item("lxc.lxcpath");
    } else {
        lxcpath = info[0].ToString().Utf8Value().c_str();
    }

    char **names;
    // Get the list of all containers
    int num_containers = list_defined_containers(lxcpath, &names, nullptr);
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
    // You can specify the LXC path or set it to NULL for the default path
    const char *lxcpath;
    if (!info[0].IsString()) {
        lxcpath = lxc_get_global_config_item("lxc.lxcpath");
    } else {
        lxcpath = info[0].ToString().Utf8Value().c_str();
    }

    char **names;
    // Get the list of all containers
    int num_containers = list_active_containers(lxcpath, &names, nullptr);
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
    return Boolean::New(info.Env(), node_lxc_has_api_extension(info[0].ToString().Utf8Value().c_str()));
}

Value _lxc_config_item_is_supported(const CallbackInfo &info) {
    if (!info[0].IsString()) {
        return Boolean::New(info.Env(), false);
    }
    return Boolean::New(info.Env(), node_lxc_config_item_is_supported(info[0].ToString().Utf8Value().c_str()));
}

//endregion

//region container

Value _lxc_container_new(const CallbackInfo &info) {
    Env env = info.Env();
    // Return a unique identifier or handle to the container
    return External<LXCContainer>::New(env, LXCContainer::New(
            env,
            info[0].IsString() ? info[0].ToString().Utf8Value().c_str() : nullptr,
            info[1].IsString() ? info[1].ToString().Utf8Value().c_str() : nullptr
    ));
}


Value _lxc_defined(const CallbackInfo &info) {
    if (!info[0].IsExternal()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();
    return Boolean::New(info.Env(), node_lxc_defined(ref.Data()->reference()));
}


Value _lxc_add_device_node(const CallbackInfo &info) {
    if (!info[0].IsExternal() || !info[1].IsString() || !info[2].IsString()) {
        TypeError::New(info.Env(), "Invalid parameters").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();
    auto c = ref.Data()->reference();
    return Boolean::New(info.Env(), node_lxc_add_device_node(
            c,
            info[0].IsString() ? info[0].ToString().Utf8Value().c_str() : nullptr,
            info[1].IsString() ? info[1].ToString().Utf8Value().c_str() : nullptr
    ));
}


Value _lxc_state(const CallbackInfo &info) {
    if (!info[0].IsExternal()) {
        TypeError::New(info.Env(), "Invalid parameters").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();
    return String::New(info.Env(), node_lxc_state(ref.Data()->reference()));
}

Value _lxc_running(const CallbackInfo &info) {
    if (!info[0].IsExternal()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();
    return Boolean::New(info.Env(), node_lxc_running(ref.Data()->reference()));
}

Value _lxc_freeze(const CallbackInfo &info) {
    if (!info[0].IsExternal()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();
    return Boolean::New(info.Env(), node_lxc_freeze(ref.Data()->reference()));
}

Value _lxc_unfreeze(const CallbackInfo &info) {
    if (!info[0].IsExternal()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();
    return Boolean::New(info.Env(), node_lxc_unfreeze(ref.Data()->reference()));
}

Value _lxc_init_pid(const CallbackInfo &info) {
    if (!info[0].IsExternal()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();
    return Number::New(info.Env(), node_lxc_init_pid(ref.Data()->reference()));
}

Value _lxc_init_pidfd(const CallbackInfo &info) {
    if (!info[0].IsExternal()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();
    return Number::New(info.Env(), node_lxc_init_pidfd(ref.Data()->reference()));
}

Value _lxc_seccomp_notify_fd(const CallbackInfo &info) {
    if (!info[0].IsExternal()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();
    return Number::New(info.Env(), node_lxc_seccomp_notify_fd(ref.Data()->reference()));
}

Value _lxc_seccomp_notify_fd_active(const CallbackInfo &info) {
    if (!info[0].IsExternal()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();
    return Number::New(info.Env(), node_lxc_seccomp_notify_fd_active(ref.Data()->reference()));
}


Value _lxc_set_timeout(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsNumber()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();
    return Number::New(info.Env(), node_lxc_set_timeout(ref.Data()->reference(), info[0].ToNumber().Int32Value()));
}


Value _lxc_devpts_fd(const CallbackInfo &info) {
    if (!info[0].IsExternal()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();
    return Number::New(info.Env(), node_lxc_devpts_fd(ref.Data()->reference()));
}

Value _lxc_want_daemonize(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsBoolean()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();
    return Boolean::New(info.Env(), node_lxc_want_daemonize(ref.Data()->reference(), info[1].ToBoolean().Value()));
}


Value _lxc_want_close_all_fds(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsBoolean()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();
    return Boolean::New(info.Env(), node_lxc_want_close_all_fds(ref.Data()->reference(), info[1].ToBoolean().Value()));
}

Value _lxc_create(const CallbackInfo &info) {
    Env env = info.Env();

    if (!info[0].IsExternal() || !info[1].IsString() || !info[2].IsString() || !info[3].IsObject() ||
        !info[4].IsNumber() || !info[5].IsArray()) {
        TypeError::New(env, "Invalid parameter").ThrowAsJavaScriptException();
        return env.Null();
    }

    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();

    // Parse container creation parameters
    std::string t = info[1].ToString().Utf8Value();
    std::string bdevtype = info[2].ToString().Utf8Value();

    bdev_specs specs{};
    specs.fstype = (char *) info[3].ToObject().Get("fstype").ToString().Utf8Value().c_str();
    specs.fssize = static_cast<uint64_t>(info[3].ToObject().Get("fssize").ToNumber().Int64Value());
    specs.zfs.zfsroot = (char *) info[3].ToObject().Get("zfs").ToObject().Get("zfsroot").ToString().Utf8Value().c_str();
    specs.lvm.vg = (char *) info[3].ToObject().Get("lvm").ToObject().Get("vg").ToString().Utf8Value().c_str();
    specs.lvm.lv = (char *) info[3].ToObject().Get("lvm").ToObject().Get("lv").ToString().Utf8Value().c_str();
    specs.lvm.thinpool = (char *) info[3].ToObject().Get("lvm").ToObject().Get(
            "thinpool").ToString().Utf8Value().c_str();
    specs.dir = (char *) info[3].ToObject().Get("dir").ToString().Utf8Value().c_str();
    specs.rbd.rbdname = (char *) info[3].ToObject().Get("rdb").ToObject().Get("rbdname").ToString().Utf8Value().c_str();
    specs.rbd.rbdpool = (char *) info[3].ToObject().Get("rdb").ToObject().Get("rbdpool").ToString().Utf8Value().c_str();

    int flags = info[4].ToNumber().Int32Value();

    // Convert JavaScript array to char* const argv[]
    Array argvArray = info[5].As<Array>();
    size_t argc = argvArray.Length();
    char **argv = new char *[argc + 1];
    for (size_t i = 0; i < argc; ++i) {
        argv[i] = strdup(argvArray.Get(i).ToString().Utf8Value().c_str());
    }
    argv[argc] = nullptr;

    // Call the C++ implementation
    bool result = node_lxc_create(ref.Data()->reference(), t.c_str(), bdevtype.c_str(), &specs, flags, argv);

    // Free allocated memory
    for (size_t i = 0; i < argc; ++i) {
        free(const_cast<char *>(argv[i]));
    }
    delete[] argv;

    return Boolean::New(env, result);
}


Value _lxc_start(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsNumber() && !info[2].IsArray()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();

    // Convert JavaScript array to char* const argv[]
    Array argvArray = info[5].As<Array>();
    size_t argc = argvArray.Length();
    char **argv = new char *[argc + 1];
    for (size_t i = 0; i < argc; ++i) {
        argv[i] = strdup(argvArray.Get(i).ToString().Utf8Value().c_str());
    }
    argv[argc] = nullptr;

    // Call the C++ implementation
    bool result = node_lxc_start(ref.Data()->reference(), info[1].ToNumber().Int32Value(), argv);

    // Free allocated memory
    for (size_t i = 0; i < argc; ++i) {
        free(const_cast<char *>(argv[i]));
    }
    delete[] argv;

    return Boolean::New(info.Env(), result);
}


Value _lxc_stop(const CallbackInfo &info) {
    if (!info[0].IsExternal()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();
    return Boolean::New(info.Env(), node_lxc_stop(ref.Data()->reference()));
}


Value _lxc_reboot(const CallbackInfo &info) {
    if (!info[0].IsExternal()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();
    return Boolean::New(info.Env(), node_lxc_reboot(ref.Data()->reference()));
}

Value _lxc_shutdown(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsNumber()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();
    return Boolean::New(info.Env(), node_lxc_shutdown(ref.Data()->reference(), info[1].ToNumber().Int32Value()));
}

Value _lxc_config_file_name(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsNumber()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();
    return String::New(info.Env(), node_lxc_config_file_name(ref.Data()->reference()));
}

Value _lxc_destroy(const CallbackInfo &info) {
    if (!info[0].IsExternal()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();
    return Boolean::New(info.Env(), node_lxc_destroy(ref.Data()->reference()));
}

Value _lxc_destroy_with_snapshots(const CallbackInfo &info) {
    if (!info[0].IsExternal()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();
    return Boolean::New(info.Env(), node_lxc_destroy_with_snapshots(ref.Data()->reference()));
}

Value _lxc_wait(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsString() && !info[2].IsNumber()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();
    return Boolean::New(info.Env(),
                        node_lxc_wait(ref.Data()->reference(), info[1].ToString().Utf8Value().c_str(),
                                      info[2].ToNumber().Int32Value())
    );
}

Value _lxc_get_config_item(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsString()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();
    return String::New(info.Env(),
                       node_lxc_get_config_item(ref.Data()->reference(), info[1].ToString().Utf8Value().c_str())
    );
}

Value _lxc_set_config_item(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsString() && !info[2].IsString()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
        return info.Env().Null();
    }
    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();
    return Boolean::New(info.Env(),
                        node_lxc_set_config_item(
                                ref.Data()->reference(),
                                info[1].ToString().Utf8Value().c_str(),
                                info[2].ToString().Utf8Value().c_str()
                        )
    );
}

void _lxc_clear_config(const CallbackInfo &info) {
    if (!info[0].IsExternal()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
    }
    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();
    node_lxc_clear_config(ref.Data()->reference());
}

Boolean _lxc_clear_config_item(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsString()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
    }
    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();
    return Boolean::New(info.Env(),
                        node_lxc_clear_config_item(ref.Data()->reference(), info[1].ToString().Utf8Value().c_str()));
}


Boolean _lxc_get_running_config_item(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsString()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
    }
    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();
    return Boolean::New(info.Env(),
                        node_lxc_get_running_config_item(ref.Data()->reference(),
                                                         info[1].ToString().Utf8Value().c_str()));
}

String _lxc_get_keys(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsString()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
    }
    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();
    return String::New(info.Env(),
                       node_lxc_get_keys(ref.Data()->reference(),
                                         info[1].ToString().Utf8Value().c_str()));
}

String _lxc_get_cgroup_item(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsString()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
    }
    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();
    return String::New(info.Env(),
                       node_lxc_get_cgroup_item(ref.Data()->reference(),
                                                info[1].ToString().Utf8Value().c_str()));
}

Boolean _lxc_set_cgroup_item(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsString() && !info[2].IsString()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
    }
    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();
    return Boolean::New(info.Env(),
                        node_lxc_set_cgroup_item(ref.Data()->reference(),
                                                 info[1].ToString().Utf8Value().c_str(),
                                                 info[2].ToString().Utf8Value().c_str()));
}


String _lxc_get_config_path(const CallbackInfo &info) {
    if (!info[0].IsExternal()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
    }
    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();
    return String::New(info.Env(),
                       node_lxc_get_config_path(ref.Data()->reference()));
}


Boolean _lxc_set_config_path(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsString()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
    }
    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();
    return Boolean::New(info.Env(),
                        node_lxc_set_config_path(ref.Data()->reference(),
                                                 info[1].ToString().Utf8Value().c_str()));
}


Boolean _lxc_load_config(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsString()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
    }
    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();
    return Boolean::New(info.Env(),
                        node_lxc_load_config(ref.Data()->reference(),
                                             info[1].ToString().Utf8Value().c_str()));
}

Boolean _lxc_save_config(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsString()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
    }
    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();
    return Boolean::New(info.Env(),
                        node_lxc_save_config(ref.Data()->reference(),
                                             info[1].ToString().Utf8Value().c_str()));
}


Boolean _lxc_clone(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsString() && !info[2].IsString() && !info[3].IsNumber() &&
        !info[4].IsString()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
    }
    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();
    return Boolean::New(info.Env(),
                        node_lxc_clone(ref.Data()->reference(),
                                       info[1].ToString().Utf8Value().c_str(),
                                       info[2].ToString().Utf8Value().c_str(),
                                       info[3].ToNumber().Int32Value(),
                                       info[4].ToString().Utf8Value().c_str()
                        )
    );
}

Number _lxc_console_getfd(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsNumber()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
    }
    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();
    return Number::New(info.Env(), node_lxc_console_getfd(ref.Data()->reference(), info[1].ToNumber().Int32Value()));
}


Boolean _lxc_console(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsNumber() && !info[2].IsNumber() && !info[3].IsNumber() &&
        !info[4].IsNumber() &&
        !info[5].IsNumber()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
    }
    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();
    return Boolean::New(info.Env(),
                        node_lxc_console(ref.Data()->reference(),
                                         info[1].ToNumber().Int32Value(),
                                         info[2].ToNumber().Int32Value(),
                                         info[3].ToNumber().Int32Value(),
                                         info[4].ToNumber().Int32Value(),
                                         info[5].ToNumber().Int32Value()
                        )
    );
}

Array _lxc_get_interfaces(const CallbackInfo &info) {
    if (!info[0].IsExternal()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
    }
    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();

    // Call the underlying C++ function
    char **interfaces = node_lxc_get_interfaces(ref.Data()->reference());

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

Array _lxc_get_ips(const CallbackInfo &info) {
    if (!info[0].IsExternal() && !info[1].IsString() && !info[2].IsString() && !info[3].IsNumber()) {
        TypeError::New(info.Env(), "Invalid parameter").ThrowAsJavaScriptException();
    }
    // Get the container handle from JavaScript
    External<LXCContainer> ref = info[0].As<External<LXCContainer>>();

    // Call the underlying C++ function
    char **ips = node_lxc_get_ips(ref.Data()->reference(), info[1].ToString().Utf8Value().c_str(),
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


//endregion

Object Init(Env env, Object exports) {
    //region general
    exports.Set(String::New(env, "version"), Function::New(env, version));
    exports.Set(String::New(env, "version_object"), Function::New(env, version_object));
    exports.Set(String::New(env, "is_devel"), Function::New(env, is_devel));
    exports.Set(String::New(env, "lxc_get_global_config_item"), Function::New(env, _lxc_get_global_config_item));
    exports.Set(String::New(env, "list_all_containers"), Function::New(env, _list_all_containers));
    exports.Set(String::New(env, "list_defined_containers"), Function::New(env, _list_defined_containers));
    exports.Set(String::New(env, "list_active_containers"), Function::New(env, _list_active_containers));
    exports.Set(String::New(env, "lxc_container_new"), Function::New(env, _lxc_container_new));
    exports.Set(String::New(env, "lxc_has_api_extension"), Function::New(env, _lxc_has_api_extension));
    exports.Set(String::New(env, "lxc_config_item_is_supported"), Function::New(env, _lxc_config_item_is_supported));
    //endregion
    //region container
    exports.Set(String::New(env, "lxc_defined"), Function::New(env, _lxc_defined));
    exports.Set(String::New(env, "lxc_state"), Function::New(env, _lxc_state));
    exports.Set(String::New(env, "lxc_running"), Function::New(env, _lxc_running));
    exports.Set(String::New(env, "lxc_freeze"), Function::New(env, _lxc_freeze));
    exports.Set(String::New(env, "lxc_unfreeze"), Function::New(env, _lxc_unfreeze));
    exports.Set(String::New(env, "lxc_init_pid"), Function::New(env, _lxc_init_pid));
    exports.Set(String::New(env, "lxc_init_pidfd"), Function::New(env, _lxc_init_pidfd));
    exports.Set(String::New(env, "lxc_seccomp_notify_fd"), Function::New(env, _lxc_seccomp_notify_fd));
    exports.Set(String::New(env, "lxc_seccomp_notify_fd_active"), Function::New(env, _lxc_seccomp_notify_fd_active));
    exports.Set(String::New(env, "lxc_set_timeout"), Function::New(env, _lxc_set_timeout));
    exports.Set(String::New(env, "lxc_devpts_fd"), Function::New(env, _lxc_devpts_fd));
    exports.Set(String::New(env, "lxc_want_daemonize"), Function::New(env, _lxc_want_daemonize));
    exports.Set(String::New(env, "lxc_want_close_all_fds"), Function::New(env, _lxc_want_close_all_fds));
    exports.Set(String::New(env, "lxc_create"), Function::New(env, _lxc_create));
    exports.Set(String::New(env, "lxc_start"), Function::New(env, _lxc_start));
    exports.Set(String::New(env, "lxc_stop"), Function::New(env, _lxc_stop));
    exports.Set(String::New(env, "lxc_reboot"), Function::New(env, _lxc_reboot));
    exports.Set(String::New(env, "lxc_shutdown"), Function::New(env, _lxc_shutdown));
    exports.Set(String::New(env, "lxc_config_file_name"), Function::New(env, _lxc_config_file_name));
    exports.Set(String::New(env, "lxc_destroy"), Function::New(env, _lxc_destroy));
    exports.Set(String::New(env, "lxc_destroy_with_snapshots"), Function::New(env, _lxc_destroy_with_snapshots));
    exports.Set(String::New(env, "lxc_wait"), Function::New(env, _lxc_wait));
    exports.Set(String::New(env, "lxc_get_config_item"), Function::New(env, _lxc_get_config_item));
    exports.Set(String::New(env, "lxc_set_config_item"), Function::New(env, _lxc_set_config_item));
    exports.Set(String::New(env, "lxc_clear_config"), Function::New(env, _lxc_clear_config));
    exports.Set(String::New(env, "lxc_clear_config_item"), Function::New(env, _lxc_clear_config_item));
    exports.Set(String::New(env, "lxc_get_running_config_item"), Function::New(env, _lxc_get_running_config_item));
    exports.Set(String::New(env, "lxc_get_keys"), Function::New(env, _lxc_get_keys));
    exports.Set(String::New(env, "lxc_get_cgroup_item"), Function::New(env, _lxc_get_cgroup_item));
    exports.Set(String::New(env, "lxc_set_cgroup_item"), Function::New(env, _lxc_set_cgroup_item));
    exports.Set(String::New(env, "lxc_get_config_path"), Function::New(env, _lxc_get_config_path));
    exports.Set(String::New(env, "lxc_set_config_path"), Function::New(env, _lxc_set_config_path));
    exports.Set(String::New(env, "lxc_load_config"), Function::New(env, _lxc_load_config));
    exports.Set(String::New(env, "lxc_save_config"), Function::New(env, _lxc_save_config));
    exports.Set(String::New(env, "lxc_clone"), Function::New(env, _lxc_clone));
    exports.Set(String::New(env, "lxc_console_getfd"), Function::New(env, _lxc_console_getfd));
    exports.Set(String::New(env, "lxc_console"), Function::New(env, _lxc_console));
    exports.Set(String::New(env, "lxc_get_interfaces"), Function::New(env, _lxc_get_interfaces));
    exports.Set(String::New(env, "lxc_get_ips"), Function::New(env, _lxc_get_ips));


    exports.Set(String::New(env, "lxc_add_device_node"), Function::New(env, _lxc_add_device_node));

    //endregion
    return exports;
}


NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
