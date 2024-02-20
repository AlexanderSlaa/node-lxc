//
// Created by root on 2/19/24.
//

#ifndef NODE_LXC_UNDEFINED_H
#define NODE_LXC_UNDEFINED_H

#include <napi.h>

class Undefined{
public:
    static Napi::Value New(
            Napi::Env env, ///< N-API environment
            void *any
    ){
        return env.Undefined();
    }
};

#endif //NODE_LXC_UNDEFINED_H
