//
// Created by root on 1/14/24.
//

#ifndef NODE_LIBVIRT_PROMISEWORKER_H
#define NODE_LIBVIRT_PROMISEWORKER_H

#include <napi.h>
#include "./Undefined.h"
#include <stdexcept>

template <typename W = Undefined, typename T = nullptr_t>
class PromiseWorker : public Napi::AsyncWorker {
public:
    PromiseWorker(const Napi::Promise::Deferred &deferred, std::function<void(PromiseWorker<W,T> *)> &&asyncFunction,
                  void *data = nullptr)
            : Napi::AsyncWorker(deferred.Env()), deferred_(deferred), asyncFunction_(std::move(asyncFunction)),
              data_(data) {}

    void Execute() override {
        try {
            asyncFunction_(this);
        } catch (const std::exception &e) {
            SetError(e.what());
        }
    }

    void Result(T val) {
        val_ = val;
    }

    void Error(const std::string &error) {
        SetError(error);
    }

    void OnOK() override {
        Napi::HandleScope scope(Env());
        deferred_.Resolve(W::New(Env(), this->val_));
    }

    void OnError(const Napi::Error &e) override {
        Napi::HandleScope scope(Env());
        deferred_.Reject(e.Value());
    }


    void *data() {
        return this->data_;
    }

private:
    Napi::Promise::Deferred deferred_;
    std::function<void(PromiseWorker<W,T> *)> asyncFunction_;
    void *data_;
    T val_;
};


#endif //NODE_LIBVIRT_PROMISEWORKER_H
