//
// Created by root on 2/11/24.
//

#ifndef NODE_LXC_ASSERT_H
#define NODE_LXC_ASSERT_H

#define assert(handle, message) \
    if(!(handle)){ \
        Napi::Error::New(info.Env(), message).ThrowAsJavaScriptException(); \
        return info.Env().Undefined(); \
    }

#define assert_void(handle, message) \
    if(!(handle)){ \
        Napi::Error::New(info.Env(), message).ThrowAsJavaScriptException(); \
        return; \
    }

#define assert_deferred(handle, message) \
    if(!(handle)){ \
        deferred.Reject(Napi::String::New(info.Env(), message)); \
        return deferred.Promise(); \
    }

#endif //NODE_LXC_ASSERT_H
