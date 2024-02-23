//
// Created by root on 1/14/24.
//

#ifndef NODE_LXC_PROMISEWORKER_H
#define NODE_LXC_PROMISEWORKER_H

#include <napi.h>
#include "./Undefined.h"
#include <stdexcept>

template<typename... Args>
class AsyncPromise : public Napi::AsyncWorker {

public:

    static Napi::Value UndefinedWrapper(AsyncPromise *worker, const std::tuple<Args...> &) {
        return worker->Env().Undefined();
    }

    static Napi::Value NullWrapper(AsyncPromise *worker, const std::tuple<Args...> &) {
        return worker->Env().Null();
    }

    static Napi::Value StringWrapper(AsyncPromise *worker, const std::tuple<std::string> &tuple) {
        return Napi::String::New(worker->Env(), std::get<0>(tuple));
    }

    static Napi::Value StringWrapper(AsyncPromise *worker, const std::tuple<char *> &tuple) {
        return Napi::String::New(worker->Env(), std::get<0>(tuple));
    }

    static Napi::Value NumberWrapper(AsyncPromise *worker, const std::tuple<double> &tuple) {
        return Napi::Number::New(worker->Env(), std::get<0>(tuple));
    }

    static Napi::Value NumberWrapper(AsyncPromise *worker, const std::tuple<int> &tuple) {
        return Napi::Number::New(worker->Env(), std::get<0>(tuple));
    }

    static Napi::Value NumberWrapper(AsyncPromise *worker, const std::tuple<float> &tuple) {
        return Napi::Number::New(worker->Env(), std::get<0>(tuple));
    }

    static Napi::Value BooleanWrapper(AsyncPromise *worker, const std::tuple<bool> &tuple) {
        return Napi::Boolean::New(worker->Env(), std::get<0>(tuple));
    }

public:
    AsyncPromise(
            const Napi::Promise::Deferred &deferred,
            std::function<void(AsyncPromise<Args...> *)> &&asyncFunction,
            std::function<Napi::Value(AsyncPromise<Args...> *, std::tuple<Args...>)> &&valueWrapper,
            void *data = nullptr
    ) :
            Napi::AsyncWorker(deferred.Env()),
            deferred_(deferred),
            asyncFunction_(std::move(asyncFunction)),
            valueWrapper_(std::move(valueWrapper)),
            data_(data) {}

    AsyncPromise(
            const Napi::Promise::Deferred &deferred,
            std::function<void(AsyncPromise *)> &&asyncFunction,
            void *data = nullptr
    ) :
            Napi::AsyncWorker(deferred.Env()), deferred_(deferred), asyncFunction_(std::move(asyncFunction)),
            data_(data) {}

    void Execute() override {
        try {
            asyncFunction_(this);
        } catch (const Napi::Error &e) {
            SetError(e.Message());
        } catch (const std::exception &e) {
            SetError(e.what());
        }
    }

    void Result(Args... args) {
        val_ = std::make_tuple(std::forward<Args>(args)...);
    }

    void Error(const std::string &error) {
        SetError(error);
    }

    void OnOK() override {
        Napi::HandleScope scope(Env());
        deferred_.Resolve(this->valueWrapper_(this, val_));
    }

    void OnError(const Napi::Error &e) override {
        Napi::HandleScope scope(Env());
        deferred_.Reject(e.Value());
    }

    void *data() {
        return this->data_;
    }

    Napi::Promise Promise() {
        return deferred_.Promise();
    }

private:
    Napi::Promise::Deferred deferred_;
    std::function<void(AsyncPromise *)> asyncFunction_;
    std::function<Napi::Value(AsyncPromise *,
                              std::tuple<Args...>)> valueWrapper_ = AsyncPromise::UndefinedWrapper;
    std::tuple<Args...> val_;
    void *data_;
};


#endif //NODE_LXC_PROMISEWORKER_H
