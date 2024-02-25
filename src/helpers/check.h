//
// Created by A.P.A.Slaa (a.p.a.slaa@projectsource.nl) on 12/16/23.
//

#ifndef NODE_LXC_CHECK_H
#define NODE_LXC_CHECK_H

#define check(condition, message) \
    if((condition)){ \
         Napi::Error::New(info.Env(), message).ThrowAsJavaScriptException(); \
         return info.Env().Undefined(); \
    }

#define check_void(condition, message) \
    if((condition)){ \
         Napi::Error::New(info.Env(), message).ThrowAsJavaScriptException(); \
         return; \
    }


#define check_deferred(condition, message) \
    if((condition)){ \
        deferred.Reject(Napi::String::New(info.Env(), message)); \
        return deferred.Promise(); \
    }

#endif //NODE_LXC_CHECK_H
