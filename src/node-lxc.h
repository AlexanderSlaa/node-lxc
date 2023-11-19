//
// Created by AlexanderSlaa on 11/19/23.
//

#ifndef NODE_LXC_NODE_LXC_H
#define NODE_LXC_NODE_LXC_H

#include <napi.h>


static Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "hello"),
                Napi::Function::New(env, Method));
    return exports;
}

NODE_API_MODULE(nodeLXC, Init)

#endif //NODE_LXC_NODE_LXC_H
