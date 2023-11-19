#include <node/node_api.h>
#include <napi.h>

#include <lxc/lxccontainer.h>
#include <lxc/version.h>

#include "LXCContainer.h"

using namespace Napi;

//region generic

String Version(const CallbackInfo &info) {
    Env env = info.Env();
    return String::New(env, std::to_string(LXC_VERSION_MAJOR) + "." + std::to_string(LXC_VERSION_MINOR) + "." +
                            std::to_string(LXC_VERSION_MICRO));
}

Value GetGlobalConfigItem(const CallbackInfo &info) {
    if (info[0].IsString()) {
        return String::New(info.Env(), lxc_get_global_config_item(info[0].ToString().Utf8Value().c_str()));
    }
    return info.Env().Null();
}

Value ContainerNames(const CallbackInfo &info) {
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

//endregion

//region container

Value CreateContainer(const CallbackInfo &info) {
    Env env = info.Env();
    // Return a unique identifier or handle to the container
    return External<LXCContainer>::New(env, LXCContainer::New(
            env,
            info[0].IsString() ? info[0].ToString().Utf8Value().c_str() : nullptr,
            info[1].IsString() ? info[1].ToString().Utf8Value().c_str() : nullptr
    ));
}

//endregion

Object Init(Env env, Object exports) {
    exports.Set(String::New(env, "GetGlobalConfigItem"), Function::New(env, GetGlobalConfigItem));
    exports.Set(String::New(env, "Version"), Function::New(env, Version));
    exports.Set(String::New(env, "ContainerNames"), Function::New(env, ContainerNames));
    exports.Set(String::New(env, "CreateContainer"), Function::New(env, CreateContainer));
    return exports;
}


NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
