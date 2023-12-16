//
// Created by alexanderslaa on 12/16/23.
//
#include <napi.h>
#include "Container.h"


Napi::Object Init(Napi::Env env, Napi::Object exports) {
  return Container::Init(env, exports);
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
