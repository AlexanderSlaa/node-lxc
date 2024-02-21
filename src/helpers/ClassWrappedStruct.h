//
// Created by root on 2/21/24.
//

#ifndef NODE_LXC_EXTERNALWRAPPED_H
#define NODE_LXC_EXTERNALWRAPPED_H

#include <napi.h>
#include "./Undefined.h"

template<typename T, typename W = Undefined, typename O = nullptr_t>
class ClassWrappedStruct {
public:
    static Napi::Value New(
            Napi::Env env, ///< N-API environment
            O *any
    ) {
        return T::New(env, {W::New(env, any)});
    }
};

#endif //NODE_LXC_EXTERNALWRAPPED_H
