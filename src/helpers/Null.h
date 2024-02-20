//
// Created by root on 2/19/24.
//

#ifndef NODE_LXC_UNDEFINED_H
#define NODE_LXC_UNDEFINED_H

#include <napi.h>

class Null{
public:
    static Napi::Value New(
            Napi::Env env, ///< N-API environment
            void *any
    ){
        return env.Null();
    }
};

#endif //NODE_LXC_UNDEFINED_H
