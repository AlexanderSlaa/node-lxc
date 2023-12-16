//
// Created by alexanderslaa on 12/16/23.
//

#include "Container.h"

Napi::Object Container::Init(Napi::Env env, Napi::Object exports) {
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
                    InstanceMethod("clearConfig", &Container::ClearConfig)
            });

    auto constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();  // Prevent the destructor, as it will be handled by N-API.
    exports.Set("Container", func);
    return exports;
}

Napi::Value Container::GetName(const Napi::CallbackInfo &info) {
    return Napi::String::New(info.Env(), _container->name);
}

Napi::Value Container::GetState(const Napi::CallbackInfo &info) {
    return Napi::String::New(info.Env(), _container->state(_container));
}



Container::Container(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Container>(info) {
    Napi::Env env = info.Env();
    if (info.Length() <= 0 || !info[0].IsString()) {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
        return;
    }
    _container = lxc_container_new(info[0].ToString().Utf8Value().c_str(),
                                   info[1].IsString() ? info[1].ToString().Utf8Value().c_str() : nullptr);
}

Napi::Value Container::Start(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    if (info.Length() <= 0 || !info[0].IsNumber() || !info[1].IsArray()) {
        Napi::TypeError::New(env, "Invalid arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    // Get the array of strings from the JavaScript side
    Napi::Array jsArray = info[1].As<Napi::Array>();

    // Allocate memory for char* pointers
    char **argv = new char *[jsArray.Length() + 1]; // +1 for the null terminator

    // Copy the strings to the allocated memory
    for (size_t i = 0; i < jsArray.Length(); ++i) {
        Napi::Value element = jsArray.Get(i);
        if (!element.IsString()) {
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

    auto res = _container->start(_container, info[0].ToNumber().Int32Value(), argv);

    // Cleanup: Free allocated memory
    for (size_t i = 0; i < jsArray.Length(); ++i) {
        delete[] argv[i];
    }
    delete[] argv;

    return Napi::Boolean::New(env, res);
}

Napi::Value Container::Stop(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    // Call the stop function
    auto res = _container->stop(_container);
    return Napi::Boolean::New(env, res);
}

Napi::Value Container::Create(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    if (info.Length() <= 4 || !info[0].IsString() || !info[1].IsString() || !info[2].IsObject() ||
        !info[3].IsNumber() || !info[4].IsArray()) {
        Napi::TypeError::New(env, "Invalid arguments").ThrowAsJavaScriptException();
        return env.Null();
    }


    // Parse container creation parameters
    char *t = info[1].IsString() ? strdup(info[1].ToString().Utf8Value().c_str()) : nullptr;
    char *bdevtype = info[2].IsString() ? strdup(info[2].ToString().Utf8Value().c_str()) : nullptr;

    // Handle struct bdev_specs (assuming it's an object with specific properties)
    Napi::Object bdevSpecsObj = info[2].As<Napi::Object>();
    // region Extract values from bdevSpecsObj and populate your struct bdev_specs
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
                               ? (char *) strdup(
                                    bdevSpecsObj.Get("zfs").ToObject().Get("zfsroot").ToString().Utf8Value().c_str())
                               : nullptr,
            },
            .lvm{
                    .vg = bdevSpecsObj.Has("lvm") && bdevSpecsObj.Get("lvm").IsObject() &&
                          bdevSpecsObj.Get("lvm").ToObject().Has("vg")
                          ? (char *) strdup(bdevSpecsObj.Get("lvm").ToObject().Get("vg").ToString().Utf8Value().c_str())
                          : nullptr,
                    .lv = bdevSpecsObj.Has("lvm") && bdevSpecsObj.Get("lvm").IsObject() &&
                          bdevSpecsObj.Get("lvm").ToObject().Has("lv")
                          ? (char *) strdup(bdevSpecsObj.Get("lvm").ToObject().Get("lv").ToString().Utf8Value().c_str())
                          : nullptr,
                    .thinpool = bdevSpecsObj.Has("lvm") && bdevSpecsObj.Get("lvm").IsObject() &&
                                bdevSpecsObj.Get("lvm").ToObject().Has("thinpool")
                                ? (char *) strdup(
                                    bdevSpecsObj.Get("lvm").ToObject().Get("thinpool").ToString().Utf8Value().c_str())
                                : nullptr},
            .dir = bdevSpecsObj.Has("dir") && bdevSpecsObj.Get("dir").IsString() ? (char *) strdup(
                    bdevSpecsObj.Get("dir").ToString().Utf8Value().c_str())
                                                                                 : nullptr,
            .rbd{
                    .rbdname = bdevSpecsObj.Has("rbd") && bdevSpecsObj.Get("rbd").IsObject() &&
                               bdevSpecsObj.Get("rbd").ToObject().Has("rbdname")
                               ? (char *) strdup(
                                    bdevSpecsObj.Get("rdb").ToObject().Get("rbdname").ToString().Utf8Value().c_str())
                               : nullptr,
                    .rbdpool = bdevSpecsObj.Has("rbd") && bdevSpecsObj.Get("rbd").IsObject() &&
                               bdevSpecsObj.Get("rbd").ToObject().Has("rbdpool")
                               ? (char *) strdup(
                                    bdevSpecsObj.Get("rdb").ToObject().Get("rbdpool").ToString().Utf8Value().c_str())
                               : nullptr,
            },
    };
    //endregion

    // Get the array of strings from the JavaScript side
    Napi::Array jsArray = info[4].As<Napi::Array>();

    // Allocate memory for char* pointers
    char **argv = new char *[jsArray.Length() + 1]; // +1 for the null terminator

    // Copy the strings to the allocated memory
    for (size_t i = 0; i < jsArray.Length(); ++i) {
        Napi::Value element = jsArray.Get(i);
        if (!element.IsString()) {
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
    auto res = _container->create(_container, t, bdevtype, &bdevSpecs, info[4].ToNumber().Int32Value(),
                                  argv);

    // Cleanup: Free allocated memory
    for (size_t i = 0; i < jsArray.Length(); ++i) {
        delete[] argv[i];
    }
    delete[] argv;
    free(t);
    free(bdevtype);

    return Napi::Boolean::New(env, res);
}

Napi::Value Container::GetConfigItem(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    if (info.Length() <= 0 || !info[0].IsString()) {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
        return env.Null();
    }

    char *value;

    int len = _container->get_config_item(_container, info[0].ToString().Utf8Value().c_str(), nullptr, 0);
    if (len <= 0) {
        return env.Null();
    }

    again:
    value = (char *) malloc(sizeof(char) * len + 1);
    if (value == nullptr)
        goto again;

    if (_container->get_config_item(_container, info[0].ToString().Utf8Value().c_str(), value, len + 1) != len) {
        free(value);
        return env.Null();
    }

    return Napi::String::New(env, value);
}

Napi::Value Container::SetConfigItem(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    if (info.Length() <= 0 || !info[0].IsString() || !info[1].IsString()) {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
        return env.Null();
    }
    return Napi::Boolean::New(env, _container->set_config_item(_container, info[0].ToString().Utf8Value().c_str(),
                                                               info[1].ToString().Utf8Value().c_str()));
}

Napi::Value Container::ClearConfigItem(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    if (info.Length() <= 0 || !info[0].IsString()) {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
        return env.Null();
    }
    return Napi::Boolean::New(env, _container->clear_config_item(_container, info[0].ToString().Utf8Value().c_str()));
}

void Container::ClearConfig(const Napi::CallbackInfo &info) {
    _container->clear_config(_container);
}

