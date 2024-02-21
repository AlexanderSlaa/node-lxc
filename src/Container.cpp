//
// Created by alexanderslaa on 12/16/23.
//

#include "Container.h"

#include <sys/wait.h>
#include <sstream>
#include "./helpers/check.h"
#include "./helpers/assert.h"
#include "./helpers/Array.h"
#include "./helpers/object.h"
#include "./helpers/ClassWrappedStruct.h"
#include "./helpers/PromiseWorker.h"


Napi::Object Container::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func =
            DefineClass(env, "Container", {
                    /* Instance Accessors */
                    InstanceAccessor("name", &Container::GetName, &Container::SetName),
                    InstanceAccessor("defined", &Container::GetDefined, nullptr),
                    InstanceAccessor("state", &Container::GetState, nullptr),
                    InstanceAccessor("running", &Container::GetRunning, nullptr),
                    InstanceAccessor("initPID", &Container::GetInitPID, nullptr),
                    InstanceAccessor("configFileName", &Container::GetConfigFileName, nullptr),
                    InstanceAccessor("daemonize", &Container::GetDaemonize, &Container::SetDaemonize),
//                    TODO: InstanceAccessor("configPath", &Container::GetConfigPath, Container::SetConfigPath),

                    /* Instance Methods */
                    InstanceMethod("freeze", &Container::Freeze),
                    InstanceMethod("unfreeze", &Container::Unfreeze),
                    InstanceMethod("loadConfig", &Container::LoadConfig),

                    InstanceMethod("start", &Container::Start),
                    InstanceMethod("stop", &Container::Stop),

                    InstanceMethod("closeAllFds", &Container::CloseAllFds),
                    InstanceMethod("wait", &Container::Wait),
                    InstanceMethod("setConfigItem", &Container::SetConfigItem),
                    InstanceMethod("destroy", &Container::Destroy),
                    InstanceMethod("save", &Container::Save),

                    InstanceMethod("create", &Container::Create),
                    InstanceMethod("reboot", &Container::Reboot),
                    InstanceMethod("shutdown", &Container::Shutdown),
                    InstanceMethod("clearConfig", &Container::ClearConfig),
                    InstanceMethod("clearConfigItem", &Container::ClearConfigItem),
                    InstanceMethod("getConfigItem", &Container::GetConfigItem),
                    InstanceMethod("getRunningConfigItem", &Container::GetRunningConfigItem),
                    InstanceMethod("getKeys", &Container::GetKeys),
                    InstanceMethod("getInterfaces", &Container::GetInterfaces),
                    InstanceMethod("getIPs", &Container::GetIPs),
                    InstanceMethod("getCGroupItem", &Container::GetCGroupItem),
                    InstanceMethod("setCGroupItem", &Container::SetCGroupItem),
                    InstanceMethod("clone", &Container::Clone),
                    InstanceMethod("consoleGetFds", &Container::ConsoleGetFds),
                    InstanceMethod("console", &Container::Console),

                    InstanceMethod("attach", &Container::Attach), // WITH run_wait
                    //TODO: InstanceMethod("snapshot",  &Container::Snapshot),
                    //TODO: InstanceMethod("snapshotList",  &Container::SnapshotList),
                    //TODO: InstanceMethod("snapshotRestore",  &Container::SnapshotRestore),
                    //TODO: InstanceMethod("snapshotDestroy",  &Container::SnapshotDestroy), //TODO: Add option to destroy all snapshots
                    //TODO: MAY CONTROL (may_control)???
                    //TODO: InstanceMethod("addDeviceNode", &Container::AddDeviceNode),
                    //TODO: InstanceMethod("removeDeviceNode", &Container::RemoveDeviceNode),
                    //TODO: InstanceMethod("attachInterface", &Container::AttachInterface),
                    //TODO: InstanceMethod("detachInterface", &Container::DetachInterface),
                    //TODO: InstanceMethod("checkpoint", &Container::Checkpoint),
                    //TODO: InstanceMethod("restore", &Container::Restore),
                    //TODO: InstanceMethod("migrate", &Container::Migrate),
                    //TODO: InstanceMethod("consoleLog", &Container::ConsoleLog),
                    //TODO: InstanceMethod("reboot2", &Container::Reboot2),
                    //TODO: InstanceMethod("mount", &Container::Mount),
                    //TODO: InstanceMethod("umount", &Container::Umount),

                    //TODO: THIS MAY NEED TO BE A GETTER
                    //TODO: InstanceMethod("seccompNotifyFd", &Container::SeccompNotifyFd),
                    //TODO: InstanceMethod("seccompNotifyFdActive", &Container::SeccompNotifyFdActive),
                    //TODO: InstanceMethod("initPIDFd", &Container::InitPIDFd),
                    //TODO: InstanceMethod("devptsFd", &Container::DevptsFd),

                    InstanceMethod("exec", &Container::Exec),
            });

    auto *constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.SetInstanceData(constructor);
    exports.Set("Container", func);
    return exports;
}

Napi::Value Container::GetName(const Napi::CallbackInfo &info) {
    assert(_container, "Invalid container pointer")
    return Napi::String::New(info.Env(), _container->name);
}

void Container::SetName(const Napi::CallbackInfo &info, const Napi::Value &value) {
    assert_void(_container, "Invalid container pointer")
    assert_void(value.IsString(), "Invalid arguments")
    check_void(_container->rename(_container, value.ToString().Utf8Value().c_str()),
               "Unable to rename container to " + value.ToString().Utf8Value())
}

Napi::Value Container::GetState(const Napi::CallbackInfo &info) {
    assert(_container, "Invalid container pointer")
    return Napi::String::New(info.Env(), _container->state(_container));
}

Napi::Value Container::GetRunning(const Napi::CallbackInfo &info) {
    assert(_container, "Invalid container pointer")
    return Napi::Boolean::New(info.Env(), _container->is_running(_container));
}


Napi::Value Container::GetInitPID(const Napi::CallbackInfo &info) {
    assert(_container, "Invalid container pointer")
    return Napi::Number::New(info.Env(), _container->init_pid(_container));
}

Napi::Value Container::GetConfigFileName(const Napi::CallbackInfo &info) {
    assert(_container, "Invalid container pointer")
    return Napi::String::New(info.Env(), _container->config_file_name(_container));
}

Napi::Value Container::GetDefined(const Napi::CallbackInfo &info) {
    assert(_container, "Invalid container pointer")
    return Napi::Boolean::New(info.Env(), _container->is_defined(_container));
}

Container::Container(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Container>(info) {
    check_void(info.Length() <= 0 || !(info[0].IsString() || info[0].IsExternal()), "Invalid argument")

    if (info[0].IsString()) {
        _container = lxc_container_new(info[0].ToString().Utf8Value().c_str(),
                                       info[1].IsString() ? info[1].ToString().Utf8Value().c_str()
                                                          : lxc_get_global_config_item("lxc.lxcpath"));
        if (info[2].IsString() && strcmp(info[2].ToString().Utf8Value().c_str(), "none") != 0) {
            _container->load_config(_container, info[2].IsString() ? info[2].ToString().Utf8Value().c_str()
                                                                   : lxc_get_global_config_item("lxc.default_config"));
        }
    } else if (info[0].IsExternal()) {
        _container = info[0].As<Napi::External<lxc_container>>().Data();
    } else {
        /* Never reached */
        Napi::Error::New(info.Env(), "Invalid constructor argument").ThrowAsJavaScriptException();
        return;
    }


}

Container::~Container() {
    if (_container) {
        // Clean up the container if needed
        lxc_container_put(_container);
    }
}

Napi::Value Container::Freeze(const Napi::CallbackInfo &info) {
    auto deferred = Napi::Promise::Deferred::New(info.Env());
    assert_deferred(_container, "Invalid container pointer")
    auto worker = new PromiseWorker<>(deferred, [this](PromiseWorker<> *worker) {
        if (!_container->freeze(_container)) {
            worker->Error("Freezing " + std::string(_container->name) + " failed");
            return;
        }
    });
    worker->Queue();
    return deferred.Promise();
}

Napi::Value Container::Unfreeze(const Napi::CallbackInfo &info) {
    auto deferred = Napi::Promise::Deferred::New(info.Env());
    assert_deferred(_container, "Invalid container pointer")
    auto worker = new PromiseWorker<>(deferred, [this](PromiseWorker<> *worker) {
        if (!_container->unfreeze(_container)) {
            worker->Error("Unfreezing " + std::string(_container->name) + " failed");
            return;
        }
    });
    worker->Queue();
    return deferred.Promise();
}

Napi::Value Container::LoadConfig(const Napi::CallbackInfo &info) {
    auto deferred = Napi::Promise::Deferred::New(info.Env());
    assert_deferred(_container, "Invalid container pointer")
    check_deferred(info.Length() <= 0 || !info[0].IsString(), "Invalid arguments")
    auto alt_file = info[0].ToString().Utf8Value();
    auto worker = new PromiseWorker<>(deferred, [this, &alt_file](PromiseWorker<> *worker) {
        if (!_container->load_config(_container, alt_file.c_str())) {
            worker->Error("Unfreezing " + std::string(_container->name) + " failed");
            return;
        }
    });
    worker->Queue();
    return deferred.Promise();
}


Napi::Value Container::Start(const Napi::CallbackInfo &info) {
    auto deferred = Napi::Promise::Deferred::New(info.Env());
    assert_deferred(_container, "Invalid container pointer")
    check_deferred(info.Length() <= 1 || !info[0].IsNumber() || !info[1].IsArray(), "Invalid arguments")

    // Get the array of strings from the JavaScript side
    auto useinit = info[0].ToNumber().Int32Value();
    auto jsArray = info[1].As<Napi::Array>();

    uint32_t argvLength;
    char **argv = Array::NapiToCharStarArray(jsArray, argvLength);

    auto worker = new PromiseWorker<Napi::Number, int>(
            deferred,
            [this, useinit, argv, argvLength](PromiseWorker<Napi::Number, int> *worker) {
                if (!this->_container->may_control(this->_container)) {
                    worker->Error("Insufficient privileges to control container");
                    return;
                }
                if (this->_container->is_running(_container)) {
                    return;
                }
                if (!_container->start(_container, useinit, argv)) {
                    worker->Error(strerror(_container->error_num));
                }
                Array::FreeCharStarArray(argv, argvLength);
            });
    worker->Queue();
    return deferred.Promise();
}

Napi::Value Container::Stop(const Napi::CallbackInfo &info) {
    auto deferred = Napi::Promise::Deferred::New(info.Env());
    assert_deferred(_container, "Invalid container pointer")
    auto worker = new PromiseWorker<>(deferred, [this](PromiseWorker<> *worker) {
        if (!_container->may_control(_container)) {
            worker->Error("Insufficient privileges to control container");
            return;
        }
        if (!_container->is_running(_container)) {
            worker->Error(std::string(this->_container->name) + " not running");
            return;
        }
        if (!_container->stop(_container)) {
            worker->Error("Container failed to stop");
            return;
        }
    });
    worker->Queue();
    return deferred.Promise();
}

void Container::SetDaemonize(const Napi::CallbackInfo &info, const Napi::Value &value) {
    assert_void(_container, "Invalid container pointer")
    assert_void(value.IsBoolean(), "Invalid argument")
    assert_void(_container->want_daemonize(_container, info[0].ToBoolean()),
                "Unable to set container wants to daemonize")
}

Napi::Value Container::GetDaemonize(const Napi::CallbackInfo &info) {
    assert(_container, "Invalid container pointer")
    return Napi::Boolean::New(info.Env(), _container->daemonize);
}

Napi::Value Container::CloseAllFds(const Napi::CallbackInfo &info) {
    assert(_container, "Invalid container pointer")
    check(info.Length() <= 0 || !info[0].IsBoolean(), "Invalid arguments")
    return Napi::Boolean::New(info.Env(), _container->want_close_all_fds(_container, info[0].ToBoolean()));
}


Napi::Value Container::Wait(const Napi::CallbackInfo &info) {
    auto deferred = Napi::Promise::Deferred::New(info.Env());
    assert_deferred(_container, "Invalid container pointer")
    check_deferred(info.Length() <= 1 || !info[0].IsString() || !info[1].IsNumber(), "Invalid arguments")

    auto state = info[0].ToString().Utf8Value();
    auto timeout = info[1].ToNumber().Int32Value();

    auto worker = new PromiseWorker<>(
            deferred,
            [this, state, timeout](PromiseWorker<> *worker) {
                if (!this->_container->may_control(this->_container)) {
                    worker->Error("Insufficient privileges to control container");
                    return;
                }
                if (!this->_container->is_running(_container)) {
                    worker->Error(std::string(this->_container->name) + " not running");
                    return;
                }
                if (!this->_container->wait(_container, state.c_str(), timeout)) {
                    worker->Error("Container timed out");
                }
            });
    worker->Queue();
    return deferred.Promise();
}


Napi::Value Container::Create(const Napi::CallbackInfo &info) {
    auto deferred = Napi::Promise::Deferred::New(info.Env());
    check_deferred(info.Length() <= 4 || !info[0].IsString() || !info[1].IsString() || !info[2].IsObject() ||
                   !info[3].IsNumber() || !info[4].IsArray(),
                   "Invalid arguments")

    // Parse container creation parameters
    char *t = info[0].IsString() ? strdup(info[0].ToString().Utf8Value().c_str()) : nullptr;
    char *bdevtype = info[1].IsString() ? strdup(info[1].ToString().Utf8Value().c_str()) : nullptr;

    // Handle struct bdev_specs (assuming it's an object with specific properties)
    Napi::Object bdevSpecsObj = info[2].As<Napi::Object>();
    // Extract values from bdevSpecsObj and populate your struct bdev_specs

//region bdev_spect
    auto *bdevSpecs = (bdev_specs *) malloc(sizeof(struct bdev_specs));

    bdevSpecs->fstype = bdevSpecsObj.Has("fstype") && bdevSpecsObj.Get("fstype").IsString() ? strdup(bdevSpecsObj.Get(
                    "fstype")
                                                                                                             .ToString()
                                                                                                             .Utf8Value()
                                                                                                             .c_str())
                                                                                            : nullptr;
    bdevSpecs->fssize = bdevSpecsObj.Has("fssize") && bdevSpecsObj.Get("fstype").IsNumber()
                        ? static_cast<uint64_t>(bdevSpecsObj.Get(
                            "fssize")
                    .ToNumber()
                    .Int64Value())
                        : 0;
    bdevSpecs->zfs.zfsroot = bdevSpecsObj.Has("zfs") && bdevSpecsObj.Get("zfs").IsObject() &&
                             bdevSpecsObj.Get("zfs").ToObject().Has("zfsroot")
                             ? (char *) strdup(
                    bdevSpecsObj.Get("zfs").ToObject().Get("zfsroot").ToString().Utf8Value().c_str())
                             : nullptr;

    bdevSpecs->lvm.vg = bdevSpecsObj.Has("lvm") && bdevSpecsObj.Get("lvm").IsObject() &&
                        bdevSpecsObj.Get("lvm").ToObject().Has("vg")
                        ? (char *) strdup(bdevSpecsObj.Get("lvm").ToObject().Get("vg").ToString().Utf8Value().c_str())
                        : nullptr;

    bdevSpecs->lvm.lv = bdevSpecsObj.Has("lvm") && bdevSpecsObj.Get("lvm").IsObject() &&
                        bdevSpecsObj.Get("lvm").ToObject().Has("lv")
                        ? (char *) strdup(bdevSpecsObj.Get("lvm").ToObject().Get("lv").ToString().Utf8Value().c_str())
                        : nullptr;

    bdevSpecs->lvm.thinpool = bdevSpecsObj.Has("lvm") && bdevSpecsObj.Get("lvm").IsObject() &&
                              bdevSpecsObj.Get("lvm").ToObject().Has("thinpool")
                              ? (char *) strdup(
                    bdevSpecsObj.Get("lvm").ToObject().Get("thinpool").ToString().Utf8Value().c_str())
                              : nullptr;

    bdevSpecs->dir = bdevSpecsObj.Has("dir") && bdevSpecsObj.Get("dir").IsString() ? (char *) strdup(
            bdevSpecsObj.Get("dir").ToString().Utf8Value().c_str())
                                                                                   : nullptr;

    bdevSpecs->rbd.rbdname = bdevSpecsObj.Has("rbd") && bdevSpecsObj.Get("rbd").IsObject() &&
                             bdevSpecsObj.Get("rbd").ToObject().Has("rbdname")
                             ? (char *) strdup(
                    bdevSpecsObj.Get("rdb").ToObject().Get("rbdname").ToString().Utf8Value().c_str())
                             : nullptr;

    bdevSpecs->rbd.rbdpool = bdevSpecsObj.Has("rbd") && bdevSpecsObj.Get("rbd").IsObject() &&
                             bdevSpecsObj.Get("rbd").ToObject().Has("rbdpool")
                             ? (char *) strdup(
                    bdevSpecsObj.Get("rdb").ToObject().Get("rbdpool").ToString().Utf8Value().c_str())
                             : nullptr;
//endregion;
    // Get the creation flags for the container
    int flags = info[3].ToNumber().Int32Value();

    // Get the array of strings from the JavaScript side
    uint32_t argvLength;
    auto argv = Array::NapiToCharStarArray(info[4].As<Napi::Array>(), argvLength);

    auto worker = new PromiseWorker<Undefined>(
            deferred,
            [this, t, argv, argvLength, bdevtype, bdevSpecs, flags](PromiseWorker<Undefined> *worker) {
                if (_container->is_defined(_container)) {
                    worker->Error("Container already exits");
                    return;
                }
                if (!_container->create(_container, t, bdevtype, bdevSpecs, flags, argv)) {
                    worker->Error(strerror(errno));
                    // Cleanup: Free allocated memory
                    Array::FreeCharStarArray(argv, argvLength);
                    free(t);
                    free(bdevtype);
                    free(bdevSpecs);
                }
            });
    worker->Queue();
    return deferred.Promise();
}

Napi::Value Container::Reboot(const Napi::CallbackInfo &info) {
    auto deferred = Napi::Promise::Deferred::New(info.Env());
    assert_deferred(_container, "Invalid container pointer");
    check_deferred(info.Length() <= 0 || !info[0].IsNumber(), "Invalid arguments")

    auto timeout = info[0].ToNumber().Int32Value();

    auto worker = new PromiseWorker<>(deferred, [this, timeout](PromiseWorker<> *worker) {
        if (!this->_container->is_running(_container)) {
            worker->Error(std::string(this->_container->name) + " not running");
            return;
        }
        if (!this->_container->reboot2(_container, timeout)) {
            worker->Error("Container reboot timed out");
        }
    });
    worker->Queue();
    return deferred.Promise();
}


Napi::Value Container::Shutdown(const Napi::CallbackInfo &info) {
    auto deferred = Napi::Promise::Deferred::New(info.Env());
    assert_deferred(_container, "Invalid container pointer");
    check_deferred(info.Length() <= 0 || !info[0].IsNumber(), "Invalid arguments")

    auto timeout = info[0].ToNumber().Int32Value();

    auto worker = new PromiseWorker<>(
            deferred,
            [this, timeout](PromiseWorker<> *worker) {
                if (!this->_container->is_running(_container)) {
                    worker->Error(std::string(this->_container->name) + " not running");
                    return;
                }
                if (!this->_container->shutdown(_container, timeout)) {
                    worker->Error("Container shutdown timed out");
                }
            });
    worker->Queue();
    return deferred.Promise();
}


Napi::Value Container::Destroy(const Napi::CallbackInfo &info) {
    auto deferred = Napi::Promise::Deferred::New(info.Env());
    assert_deferred(_container, "Invalid container pointer")
    check_deferred(info.Length() <= 0 || !info[0].IsObject(), "Invalid arguments")

    auto force = info[0].ToObject().Has("force") && info[0].ToObject().Get("force").ToBoolean();
    auto include_snapshots =
            info[0].ToObject().Has("include_snapshots") && info[0].ToObject().Get("include_snapshots").ToBoolean();
    auto worker = new PromiseWorker<>(
            deferred,
            [this, force, include_snapshots](PromiseWorker<> *worker) {

                //TODO: Check if container has snapshots

                if (_container->is_running(_container)) {
                    if (!force) {
                        worker->Error(std::string(_container->name) + " is running");
                        return;
                    }
                    /* If the container was ephemeral it will be removed on shutdown. */
                    if (!_container->stop(_container)) {
                        worker->Error("Failed to stop " + std::string(_container->name));
                        return;
                    }
                }
                /* If the container was ephemeral we have already removed it when we stopped it. */
                if (_container->is_defined(_container)) {
                    char buf[256];
                    auto ret = _container->get_config_item(_container, "lxc.ephemeral", buf, 256);
                    if (ret > 0 && strcmp(buf, "0") == 0) {
                        if (include_snapshots) {
                            if (!_container->destroy_with_snapshots(_container)) {
                                worker->Error("Destroying " + std::string(_container->name) + " failed");
                                return;
                            }
                            _container = nullptr;
                        } else {
                            if (!_container->destroy(_container)) {
                                worker->Error("Destroying " + std::string(_container->name) + " failed");
                                return;
                            }
                            _container = nullptr;
                        }

                    }
                } else {
                    _container = nullptr;
                }
            });
    worker->Queue();
    return deferred.Promise();
}


Napi::Value Container::Save(const Napi::CallbackInfo &info) {
    auto deferred = Napi::Promise::Deferred::New(info.Env());
    assert_deferred(_container, "Invalid container pointer")
    check_deferred(info.Length() <= 0 || !info[0].IsString(), "Invalid arguments")

    auto alt_file = info[0].ToString().Utf8Value();

    auto worker = new PromiseWorker<>(deferred, [this, alt_file](PromiseWorker<> *worker) {
        if (!_container->save_config(_container, alt_file.c_str())) {
            worker->Error("Container config could not be saved");
            return;
        }
    });
    worker->Queue();
    return deferred.Promise();
}


Napi::Value Container::GetConfigItem(const Napi::CallbackInfo &info) {

    assert(_container, "Invalid container pointer")
    check(info.Length() <= 0 || !info[0].IsString(), "Invalid arguments")

    char *value;

    int len = _container->get_config_item(_container, info[0].ToString().Utf8Value().c_str(), nullptr, 0);
    if (len <= 0) {
        return info.Env().Null();
    }

    again:
    value = (char *) malloc(sizeof(char) * len + 1);
    if (value == nullptr)
        goto again;

    if (_container->get_config_item(_container, info[0].ToString().Utf8Value().c_str(), value, len + 1) != len) {
        free(value);
        return info.Env().Null();
    }
    return Napi::String::New(info.Env(), value);
}

Napi::Value Container::GetRunningConfigItem(const Napi::CallbackInfo &info) {
    assert(_container, "Invalid container pointer")
    check(info.Length() <= 0 || !info[0].IsString(), "Invalid arguments")
    return Napi::String::New(info.Env(),
                             _container->get_running_config_item(_container, info[0].ToString().Utf8Value().c_str()));
}

Napi::Value Container::GetKeys(const Napi::CallbackInfo &info) {
    assert(_container, "Invalid container pointer")
    check(info.Length() <= 0 || !info[0].IsString(), "Invalid arguments")

    auto keys = Napi::Array::New(info.Env());

    char *value;
    int len = _container->get_keys(_container, info[0].ToString().Utf8Value().c_str(), nullptr, 0);
    if (len <= 0) {
        return keys;
    }

    again:
    value = (char *) malloc(sizeof(char) * len + 1);
    if (value == nullptr)
        goto again;

    if (_container->get_keys(_container, info[0].ToString().Utf8Value().c_str(), value, len + 1) != len) {
        Napi::Error::New(info.Env(), "Key amount mismatch on retrieval").ThrowAsJavaScriptException();
        free(value);
    }
//region split string /n
    std::string s(value);
    std::stringstream ss(s);
    std::string item;
    int index = 0;
    while (getline(ss, item, '\n')) {
        keys.Set(index++, Napi::String::New(info.Env(), item));
    }
//endregion
    return keys;
}

Napi::Value Container::GetInterfaces(const Napi::CallbackInfo &info) {
    auto deferred = Napi::Promise::Deferred::New(info.Env());
    assert_deferred(_container, "Invalid container pointer")

    auto worker = new PromiseWorker<Array, char **>(
            deferred,
            [this](PromiseWorker<Array, char **> *worker) {
                if (!_container->is_running(_container)) {
                    worker->Error(std::string(this->_container->name) + " not running");
                    return;
                }
                auto interfaces = _container->get_interfaces(_container);
                if (!interfaces) {
                    worker->Error("Interfaces could not be retrieved");
                    return;
                }
                worker->Result(interfaces);
            });
    worker->Queue();
    return deferred.Promise();
}

Napi::Value Container::GetIPs(const Napi::CallbackInfo &info) {
    auto deferred = Napi::Promise::Deferred::New(info.Env());
    assert_deferred(_container, "Invalid container pointer")
    check_deferred(info.Length() <= 0 || !info[0].IsString() || !info[1].IsString(), "Invalid arguments")

    auto iface = info[0].ToString().Utf8Value();
    auto family = info[1].ToString().Utf8Value();
    auto scope = (family == "inet6" && info[2].IsNumber()) ? info[2].ToNumber().Int32Value() : -1;

    auto worker = new PromiseWorker<Array, char **>(
            deferred,
            [this, iface, family, scope](PromiseWorker<Array, char **> *worker) {
                if (!_container->is_running(_container)) {
                    worker->Error("Container is not running");
                    return;
                }
                auto interfaces = _container->get_ips(_container, iface.c_str(), family.c_str(), scope);
                if (!interfaces) {
                    worker->Error("IPs for interface " + iface + " could not be retrieved");
                    return;
                }
                worker->Result(interfaces);
            });
    worker->Queue();
    return deferred.Promise();
}

Napi::Value Container::GetCGroupItem(const Napi::CallbackInfo &info) {
    assert(_container, "Invalid container pointer")
    check(info.Length() <= 0 || !info[0].IsString(), "Invalid arguments")

    char *value;
    int len = _container->get_cgroup_item(_container, info[0].ToString().Utf8Value().c_str(), nullptr, 0);
    if (len <= 0) {
        return info.Env().Undefined();
    }

    again:
    value = (char *) malloc(sizeof(char) * len + 1);
    if (value == nullptr)
        goto again;

    if (_container->get_cgroup_item(_container, info[0].ToString().Utf8Value().c_str(), value, len + 1) != len) {
        free(value);
        return info.Env().Undefined();
    }
    return Napi::String::New(info.Env(), value);
}


void Container::SetCGroupItem(const Napi::CallbackInfo &info) {
    assert_void(_container, "Invalid container pointer")
    check_void(info.Length() <= 0 || !info[0].IsString() || !info[1].IsString(), "Invalid arguments")
    assert_void(_container->set_cgroup_item(_container, info[0].ToString().Utf8Value().c_str(),
                                            info[1].ToString().Utf8Value().c_str()), "Unable to set cgroup value")
}

Napi::Value Container::Clone(const Napi::CallbackInfo &info) {
    auto deferred = Napi::Promise::Deferred::New(info.Env());
    assert_deferred(_container, "Invalid container pointer")
    check_deferred(info.Length() <= 0 || !info[0].IsObject(), "Invalid arguments")

    auto options = info[0].ToObject();

    //TODO Update check with optional
    // check_deferred(
    //         !(options.Has("newname") && options.Get("newname").IsString()) ||
    //         !(options.Has("lxcpath") && options.Get("lxcpath").IsString()) ||
    //         !(options.Has("flags") && options.Get("flags").IsNumber()) ||
    //         !(options.Has("bdevtype") && options.Get("bdevtype").IsString()) ||
    //         !(options.Has("bdevdata") && options.Get("bdevdata").IsString()) ||
    //         !(options.Has("newsize") && options.Get("newsize").IsNumber()) ||
    //         !(options.Has("hookargs") && options.Get("hookargs").IsArray()),
    //         "Invalid arguments")


    auto newname = opt_obj_val("newname", ToString().Utf8Value(), "");
    auto lxcpath = opt_obj_val("lxcpath", ToString().Utf8Value(), "");
    auto flags = opt_obj_val("flags", ToNumber().Int32Value(), 0);
    auto bdevtype = opt_obj_val("bdevtype", ToString().Utf8Value(), "");
    auto bdevdata = opt_obj_val("bdevdata", ToString().Utf8Value(), "");
    auto newsize = opt_obj_val("bdevdata", ToNumber().Int64Value(), 0);

    char **hookargs = nullptr;
    uint32_t hookargsLength = 0;
    if (options.Has("hookargs")) {
        hookargs = Array::NapiToCharStarArray(options.Get("hookargs").As<Napi::Array>(), hookargsLength);
    }

    auto worker = new PromiseWorker<ClassWrappedStruct<Container, Napi::External<lxc_container>, lxc_container>, lxc_container *>(
            deferred,
            [this, newname, lxcpath, flags, bdevtype, bdevdata, newsize, hookargsLength, hookargs](
                    PromiseWorker<ClassWrappedStruct<Container, Napi::External<lxc_container>, lxc_container>, lxc_container *> *worker) {
                if (_container->is_running(_container)) {
                    worker->Error("Container needs to be stopped to clone");
                    Array::FreeCharStarArray(hookargs, hookargsLength);
                    return;
                }
                auto clone = _container->clone(_container,
                                               (!newname.empty() ? newname.c_str() : nullptr),
                                               (!lxcpath.empty() ? lxcpath.c_str() : nullptr),
                                               flags,
                                               (!bdevtype.empty() ? bdevtype.c_str() : nullptr),
                                               (!bdevdata.empty() ? bdevdata.c_str() : nullptr),
                                               newsize,
                                               hookargs
                );
                if (!clone) {
                    worker->Error("Unable to clone container");
                } else {
                    worker->Result(clone);
                }
                Array::FreeCharStarArray(hookargs, hookargsLength);
            });
    worker->Queue();
    return deferred.Promise();
}

Napi::Value Container::ConsoleGetFds(const Napi::CallbackInfo &info) {
    auto deferred = Napi::Promise::Deferred::New(info.Env());
    assert_deferred(_container, "Invalid container point")

    int _ttynum = info[0].IsNumber() ? info[0].ToNumber().Int32Value() : -1;


    auto worker = new PromiseWorker<Array, int**>(
            deferred,
            [this, _ttynum](PromiseWorker<Array, int **> *worker) {
                if (!_container->is_running(_container)) {
                    worker->Error("Container is not running");
                    return;
                }
                int ptxfd;
                int ttynum = _ttynum;
                auto ttyfd = _container->console_getfd(_container, &ttynum, &ptxfd);
                if (ttyfd < 0) {
                    worker->Error("Unable to allocate console tty");
                    return;
                }
                auto fds = new int[]{ttyfd, ptxfd};
                worker->Result(&fds);
            });
    worker->Queue();
    return deferred.Promise();

}

void Container::SetConfigItem(const Napi::CallbackInfo &info) {
    assert_void(_container, "Invalid container pointer")
    check_void(info.Length() <= 0 || !info[0].IsString() || !info[1].IsString(), "Invalid arguments")
    assert_void(_container->set_config_item(_container, info[0].ToString().Utf8Value().c_str(),
                                            info[1].ToString().Utf8Value().c_str()), "Unable to set config item")
}

void Container::ClearConfigItem(const Napi::CallbackInfo &info) {
    assert_void(_container, "Invalid container pointer")
    check_void(info.Length() <= 0 || !info[0].IsString(), "Invalid arguments")
    assert_void(_container->clear_config_item(_container, info[0].ToString().Utf8Value().c_str()),
                "Unable to clean config item")
}

void Container::ClearConfig(const Napi::CallbackInfo &info) {
    assert_void(_container, "Invalid container pointer")
    _container->clear_config(_container);
}

int wait_for_pid_status(pid_t pid) {
    int status, ret;

    again:
    ret = waitpid(pid, &status, 0);
    if (ret == -1) {
        if (errno == EINTR)
            goto again;
        return -1;
    }
    if (ret != pid)
        goto again;
    return status;
}

Napi::Value Container::Attach(const Napi::CallbackInfo &info) {
    auto deferred = Napi::Promise::Deferred::New(info.Env());
    assert_deferred(_container, "Invalid container pointer")
    check_deferred(info.Length() <= 0 ||
                   !info[0].IsBoolean() || // CLEAN_ENV
                   !info[1].IsNumber() ||  // NAMESPACE
                   !info[2].IsNumber() ||  // PERSIONALITY
                   !info[3].IsNumber() ||  // UID
                   !info[4].IsNumber() ||  // GUID
                   !info[5].IsArray() ||   // GROUPS
                   !info[6].IsArray() ||   // STDIO
                   !info[7].IsString() ||  // CWD
                   !info[8].IsArray() ||   // ENV
                   !info[9].IsArray() ||   // KEEP_ENV
                   !info[10].IsNumber(),   // FLAGS
                   "Invalid arguments")

    auto *attach_options = (lxc_attach_options_t *) malloc(sizeof(struct lxc_attach_options_t));
    attach_options->attach_flags = info[10].ToNumber().Int32Value();

    attach_options->env_policy = LXC_ATTACH_KEEP_ENV;
    if (info[0].ToBoolean()) {
        attach_options->env_policy = LXC_ATTACH_CLEAR_ENV;
    }

    attach_options->namespaces = info[1].ToNumber().Int32Value();
    attach_options->personality = info[2].ToNumber().Int32Value();

    attach_options->uid = info[3].ToNumber().Int32Value();
    attach_options->gid = info[4].ToNumber().Int32Value();
#if VERSION_AT_LEAST(4, 0, 9)
    Napi::Array groups_array = info[5].As<Napi::Array>();
    size_t groups_array_len = groups_array.Length();
    lxc_groups_t groups = {
            .size = groups_array_len,
            .list = groups_array_len > 0 ? new gid_t[groups_array_len] : nullptr};
    if (groups.list != nullptr) {
        for (size_t i = 0; i < groups_array_len; ++i) {
            groups_array[i] = strdup(groups_array.Get(i).ToString().Utf8Value().c_str());
        }
    }

    if (groups.size > 0) {
        attach_options->groups = groups;
        attach_options->attach_flags &= LXC_ATTACH_SETGROUPS;
    }
#endif

    attach_options->stdin_fd = info[6].As<Napi::Array>().Get((uint32_t) 0).ToNumber().Int32Value(); // STDINFD
    attach_options->stdout_fd = info[6].As<Napi::Array>().Get((uint32_t) 1).ToNumber().Int32Value();
    attach_options->stderr_fd = info[6].As<Napi::Array>().Get((uint32_t) 2).ToNumber().Int32Value();

    attach_options->log_fd = -EBADF;
    attach_options->lsm_label = nullptr;

    attach_options->initial_cwd = strdup(info[7].ToString().Utf8Value().c_str());

    // Allocate memory for char* pointers
    uint32_t extra_env_varsLength;
    auto **extra_env_vars = Array::NapiToCharStarArray(info[8].As<Napi::Array>(), extra_env_varsLength);


    attach_options->extra_env_vars = extra_env_vars;

    // Allocate memory for char* pointers
    uint32_t extra_keep_envLength = 0;
    auto **extra_keep_env = Array::NapiToCharStarArray(info[9].As<Napi::Array>(), extra_env_varsLength);

    attach_options->extra_keep_env = extra_keep_env;

    auto worker = new PromiseWorker<Napi::Number, int>(
            deferred,
            [this, attach_options, extra_env_vars, extra_env_varsLength, extra_keep_env, extra_keep_envLength](
                    PromiseWorker<Napi::Number, int> *worker) {
                pid_t pid;
                int ret = _container->attach(_container, lxc_attach_run_shell, nullptr, attach_options, &pid);
                if (ret < 0) {
                    worker->Error(strerror(errno));
                }
                ret = wait_for_pid_status(pid);
                if (ret < 0) {
                    goto end;
                }
                if (WIFEXITED(ret)) {
                    ret = WEXITSTATUS(ret);
                    goto end;
                }
                end:
                // Cleanup: Free allocated memory
                Array::FreeCharStarArray(extra_env_vars, extra_env_varsLength);
                Array::FreeCharStarArray(extra_keep_env, extra_keep_envLength);
                free(attach_options);
                worker->Result(pid);
                // TODO: Probably free group items as well;
            });
    worker->Queue();
    return deferred.Promise();
}

Napi::Value Container::Exec(const Napi::CallbackInfo &info) {
    auto deferred = Napi::Promise::Deferred::New(info.Env());
    assert_deferred(_container, "Invalid container pointer")
    check_deferred(info.Length() <= 0 ||
                   !info[0].IsBoolean() || // CLEAN_ENV
                   !info[1].IsNumber() ||  // NAMESPACE
                   !info[2].IsNumber() ||  // PERSIONALITY
                   !info[3].IsNumber() ||  // UID
                   !info[4].IsNumber() ||  // GUID
                   !info[5].IsArray() ||   // GROUPS
                   !info[6].IsArray() ||   // STDIO
                   !info[7].IsString() ||  // CWD
                   !info[8].IsArray() ||   // ENV
                   !info[9].IsArray() ||   // KEEP_ENV
                   !info[10].IsNumber() ||    // FLAGS
                   !info[11].IsArray(),   // ARGV
                   "Invalid arguments")

    auto *attach_options = (lxc_attach_options_t *) malloc(sizeof(struct lxc_attach_options_t));
    attach_options->attach_flags = info[10].ToNumber().Int32Value();

    attach_options->env_policy = LXC_ATTACH_KEEP_ENV;
    if (info[0].ToBoolean()) {
        attach_options->env_policy = LXC_ATTACH_CLEAR_ENV;
    }

    attach_options->namespaces = info[1].ToNumber().Int32Value();
    attach_options->personality = info[2].ToNumber().Int32Value();

    attach_options->uid = info[3].ToNumber().Int32Value();
    attach_options->gid = info[4].ToNumber().Int32Value();
#if VERSION_AT_LEAST(4, 0, 9)
    Napi::Array groups_array = info[5].As<Napi::Array>();
    size_t groups_array_len = groups_array.Length();
    lxc_groups_t groups = {
            .size = groups_array_len,
            .list = groups_array_len > 0 ? new gid_t[groups_array_len] : nullptr};
    if (groups.list != nullptr) {
        for (size_t i = 0; i < groups_array_len; ++i) {
            groups_array[i] = strdup(groups_array.Get(i).ToString().Utf8Value().c_str());
        }
    }

    if (groups.size > 0) {
        attach_options->groups = groups;
        attach_options->attach_flags &= LXC_ATTACH_SETGROUPS;
    }
#endif

    attach_options->stdin_fd = info[6].As<Napi::Array>().Get((uint32_t) 0).ToNumber().Int32Value(); // STDINFD
    attach_options->stdout_fd = info[6].As<Napi::Array>().Get((uint32_t) 1).ToNumber().Int32Value();
    attach_options->stderr_fd = info[6].As<Napi::Array>().Get((uint32_t) 2).ToNumber().Int32Value();

    attach_options->log_fd = -EBADF;
    attach_options->lsm_label = nullptr;

    attach_options->initial_cwd = strdup(info[7].ToString().Utf8Value().c_str());

    // Allocate memory for char* pointers
    uint32_t extra_env_varsLength;
    auto **extra_env_vars = Array::NapiToCharStarArray(info[8].As<Napi::Array>(), extra_env_varsLength);


    attach_options->extra_env_vars = extra_env_vars;

    // Allocate memory for char* pointers
    uint32_t extra_keep_envLength;
    auto **extra_keep_env = Array::NapiToCharStarArray(info[9].As<Napi::Array>(), extra_keep_envLength);

    attach_options->extra_keep_env = extra_keep_env;

    auto *command = (lxc_attach_command_t *) malloc(sizeof(struct lxc_attach_command_t));
    uint32_t argvLength;
    auto argv = Array::NapiToCharStarArray(info[11].As<Napi::Array>(), argvLength);
    command->program = argv[0];
    command->argv = argv;

    auto worker = new PromiseWorker<Napi::Number, int>(
            deferred,
            [this, command, argv, argvLength, attach_options, extra_env_vars, extra_env_varsLength, extra_keep_env, extra_keep_envLength](
                    PromiseWorker<Napi::Number, int> *worker) {
                pid_t pid;
                if (_container->attach(_container, lxc_attach_run_command, command, attach_options, &pid) < 0) {
                    worker->Error(strerror(errno));
                }
                // Cleanup: Free allocated memory
                Array::FreeCharStarArray(extra_env_vars, extra_env_varsLength);
                Array::FreeCharStarArray(extra_keep_env, extra_keep_envLength);
                Array::FreeCharStarArray(argv, argvLength);
                free(attach_options);
                free(command);
                worker->Result(pid);
            });
    worker->Queue();
    return deferred.Promise();
}


Napi::Value Container::Console(const Napi::CallbackInfo &info) {
    auto deferred = Napi::Promise::Deferred::New(info.Env());
    assert_deferred(_container, "Invalid container pointer")
    check_deferred(info.Length() <= 0 || !info[0].IsNumber() || !info[1].IsArray() || !info[2].IsNumber(),
                   "Invalid arguments")

    auto ttynum = info[0].ToNumber().Int32Value();
    auto _stdin = info[1].As<Napi::Array>().Get((uint32_t) 0).ToNumber().Int32Value();
    auto _stdout = info[1].As<Napi::Array>().Get((uint32_t) 1).ToNumber().Int32Value();
    auto _stderr = info[1].As<Napi::Array>().Get((uint32_t) 2).ToNumber().Int32Value();
    auto escape = info[2].ToNumber().Int32Value();

    auto worker = new PromiseWorker<Napi::Number, int>(
            deferred,
            [this, ttynum, _stdin, _stdout, _stderr, escape](PromiseWorker<Napi::Number, int> *worker) {
                if (!this->_container->may_control(this->_container)) {
                    worker->Error("Insufficient privileges to control container");
                    return;
                }
                if (!this->_container->is_running(_container)) {
                    worker->Error(std::string(this->_container->name) + " not running");
                    return;
                }
                int ret = this->_container->console(_container, ttynum, _stdin, _stdout, _stderr, escape);
                if (ret < 0) {
                    worker->Error(strerror(_container->error_num));
                }
            });
    worker->Queue();
    return deferred.Promise();
}

Napi::Object Container::New(Napi::Env env, const std::initializer_list<napi_value> &args) {
    Napi::EscapableHandleScope scope(env);
    Napi::Object obj = env.GetInstanceData<Napi::FunctionReference>()->New(args);
    return scope.Escape(napi_value(obj)).ToObject();
}



