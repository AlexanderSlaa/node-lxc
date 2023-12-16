//
// Created by alexanderslaa on 12/16/23.
//

#ifndef NODE_LXC_CONTAINER_H
#define NODE_LXC_CONTAINER_H

#include <napi.h>
#include <lxc/lxccontainer.h>


class Container : public Napi::ObjectWrap<Container> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);

    explicit Container(const Napi::CallbackInfo &info);

    Napi::Value GetName(const Napi::CallbackInfo &info);
    Napi::Value GetState(const Napi::CallbackInfo &info);

private:
    Napi::Value Start(const Napi::CallbackInfo &info);

    Napi::Value Stop(const Napi::CallbackInfo &info);

    Napi::Value Create(const Napi::CallbackInfo &info);

    Napi::Value GetConfigItem(const Napi::CallbackInfo &info);

    Napi::Value SetConfigItem(const Napi::CallbackInfo &info);

    Napi::Value ClearConfigItem(const Napi::CallbackInfo &info);

    void ClearConfig(const Napi::CallbackInfo &info);

    struct lxc_container *_container;
};


#endif //NODE_LXC_CONTAINER_H
