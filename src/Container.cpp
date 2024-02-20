//
// Created by alexanderslaa on 12/16/23.
//

#include "Container.h"

#include <sys/wait.h>
#include "./helpers/check.h"
#include "./helpers/assert.h"
#include "./helpers/array.h"
#include "./helpers/promise_worker.h"


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
                    //TODO: InstanceAccessor("configPath", &Container::GetConfigPath, Container::SetConfigPath),

                    /* Instance Methods */
                    InstanceMethod("freeze", &Container::Freeze),
                    InstanceMethod("unfreeze", &Container::Unfreeze),
                    InstanceMethod("loadConfig", &Container::LoadConfig),

                    InstanceMethod("start", &Container::Start),
                    InstanceMethod("stop", &Container::Stop),

                    InstanceMethod("daemonize", &Container::Daemonize),
                    InstanceMethod("closeAllFds", &Container::CloseAllFds),
                    InstanceMethod("wait", &Container::Wait),
                    InstanceMethod("setConfigItem", &Container::SetConfigItem),
                    InstanceMethod("destroy", &Container::Destroy), //TODO: Add option with all snapshots
                    InstanceMethod("save", &Container::Save),

                    InstanceMethod("create", &Container::Create),
                    InstanceMethod("reboot", &Container::Reboot),
                    InstanceMethod("shutdown", &Container::Shutdown),
                    InstanceMethod("clearConfig", &Container::ClearConfig),
                    InstanceMethod("clearConfigItem", &Container::ClearConfigItem),
                    InstanceMethod("getConfigItem", &Container::GetConfigItem),
                    //TODO: InstanceMethod("getRunningConfigItem", &Container::GetRunningConfigItem),
                    //TODO: InstanceMethod("getKeys", &Container::GetKeys),
                    //TODO: InstanceMethod("getInterfaces", &Container::GetInterfaces),
                    //TODO: InstanceMethod("getIPs", &Container::GetIPs),
                    //TODO: InstanceMethod("getCGroupItem", &Container::GetCGroupItem),
                    //TODO: InstanceMethod("setCGroupItem", &Container::SetCGroupItem),
                    //TODO: InstanceMethod("clone", &Container::Clone),
                    //TODO: InstanceMethod("consoleGetFds", &Container::ConsoleGetFds),
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

    auto constructor = Napi::Persistent(func);
    constructor.SuppressDestruct(); // Prevent the destructor, as it will be handled by N-API.
    exports.Set("Container", func);
    return exports;
}

Napi::Value Container::GetName(const Napi::CallbackInfo &info) {
    assert(_container, "Invalid container pointer")
    return Napi::String::New(info.Env(), _container->name);
}

void Container::SetName(const Napi::CallbackInfo &info, const Napi::Value &value) {
    assert_void(_container, "Invalid container pointer")
    check_void(value.IsString(), "Invalid arguments")
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
    Napi::Env env = info.Env();
    if (info.Length() <= 0 || !info[0].IsString()) {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
        return;
    }
    _container = lxc_container_new(info[0].ToString().Utf8Value().c_str(),
                                   info[1].IsString() ? info[1].ToString().Utf8Value().c_str()
                                                      : lxc_get_global_config_item("lxc.lxcpath"));
    if (info[2].IsString() && strcmp(info[2].ToString().Utf8Value().c_str(), "none") != 0) {
        _container->load_config(_container, info[2].IsString() ? info[2].ToString().Utf8Value().c_str()
                                                               : lxc_get_global_config_item("lxc.default_config"));
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

    char **argv = napiArrayToCharStarArray(jsArray);
    auto argvLength = jsArray.Length();

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
                freeCharStarArray(argv, argvLength);
            });
    worker->Queue();
    return deferred.Promise();
}

Napi::Value Container::Stop(const Napi::CallbackInfo &info) {
    auto deferred = Napi::Promise::Deferred::New(info.Env());
    assert_deferred(_container, "Invalid container pointer")
    auto worker = new PromiseWorker<>(deferred, [this](PromiseWorker<> *worker) {
        if(!_container->may_control(_container)){
            worker->Error("Insufficient privileges to control container");
            return;
        }
        if(!_container->is_running(_container)){
            worker->Error("Container not running");
            return;
        }
        if(!_container->stop(_container)){
            worker->Error("Container failed to stop");
            return;
        }
    });
    worker->Queue();
    return deferred.Promise();
}

Napi::Value Container::Daemonize(const Napi::CallbackInfo &info) {
    assert(_container, "Invalid container pointer")
    check(info.Length() <= 0 || !info[0].IsBoolean(), "Invalid arguments")
    return Napi::Boolean::New(info.Env(), _container->want_daemonize(_container, info[0].ToBoolean()));
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
    Napi::Array jsArray = info[4].As<Napi::Array>();

    char **argv = napiArrayToCharStarArray(jsArray);
    auto argvLength = jsArray.Length();

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
                    freeCharStarArray(argv, argvLength);
                    free(t);
                    free(bdevtype);
                    free(bdevSpecs);
                }
            });
    worker->Queue();
    return deferred.Promise();
}

Napi::Value Container::Destroy(const Napi::CallbackInfo &info) {
    auto deferred = Napi::Promise::Deferred::New(info.Env());
    assert_deferred(_container, "Invalid container pointer")
    check_deferred(info.Length() <= 0 || !info[0].IsBoolean(), "Invalid arguments")

    auto force = info[0].ToBoolean();
    auto worker = new PromiseWorker<>(
            deferred,
            [this, force](PromiseWorker<> *worker) {

                //TODO: Check if container has snapshots

                if (_container->is_running(_container)) {
                    if (!force) {
                        worker->Error(std::string(_container->name) + " is running");
                        return;
                    }
                    /* If the container was ephemeral it will be removed on shutdown. */
                    _container->stop(_container);
                }
                /* If the container was ephemeral we have already removed it when we stopped it. */
                if (_container->is_defined(_container)) {
                    char buf[256];
                    auto ret = _container->get_config_item(_container, "lxc.ephemeral", buf, 256);
                    if (ret > 0 && strcmp(buf, "0") == 0) {
                        if (!_container->destroy(_container)) {
                            worker->Error("Destroying " + std::string(_container->name) + " failed");
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

Napi::Value Container::SetConfigItem(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    if (info.Length() <= 0 || !info[0].IsString() || !info[1].IsString()) {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
        return env.Null();
    }
    return Napi::Boolean::New(env, _container->set_config_item(_container, info[0].ToString().Utf8Value().c_str(),
                                                               info[1].ToString().Utf8Value().c_str()));
}

Napi::Value Container::ClearConfigItem(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    if (info.Length() <= 0 || !info[0].IsString()) {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
        return env.Null();
    }
    return Napi::Boolean::New(env, _container->clear_config_item(_container, info[0].ToString().Utf8Value().c_str()));
}

void Container::ClearConfig(const Napi::CallbackInfo &info) {
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

    auto js_extra_env_vars = info[8].As<Napi::Array>();

    // Allocate memory for char* pointers
    auto **extra_env_vars = napiArrayToCharStarArray(js_extra_env_vars);
    auto extra_env_varsLength = info[8].As<Napi::Array>().Length();

    attach_options->extra_env_vars = extra_env_vars;

    // Get the array of strings from the JavaScript side
    Napi::Array js_extra_keep_env = info[9].As<Napi::Array>();

    // Allocate memory for char* pointers
    auto **extra_keep_env = napiArrayToCharStarArray(js_extra_keep_env);
    auto extra_keep_envLength = info[9].As<Napi::Array>().Length();

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
                freeCharStarArray(extra_env_vars, extra_env_varsLength);
                freeCharStarArray(extra_keep_env, extra_keep_envLength);
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

    auto js_extra_env_vars = info[8].As<Napi::Array>();

    // Allocate memory for char* pointers
    auto **extra_env_vars = napiArrayToCharStarArray(js_extra_env_vars);
    auto extra_env_varsLength = info[8].As<Napi::Array>().Length();

    attach_options->extra_env_vars = extra_env_vars;

    // Get the array of strings from the JavaScript side
    Napi::Array js_extra_keep_env = info[9].As<Napi::Array>();

    // Allocate memory for char* pointers
    auto **extra_keep_env = napiArrayToCharStarArray(js_extra_keep_env);
    auto extra_keep_envLength = info[9].As<Napi::Array>().Length();

    attach_options->extra_keep_env = extra_keep_env;

    auto *command = (lxc_attach_command_t *) malloc(sizeof(struct lxc_attach_command_t));
    auto argv = napiArrayToCharStarArray(info[11].As<Napi::Array>());
    command->program = argv[0];
    command->argv = argv;

    auto worker = new PromiseWorker<Napi::Number, int>(
            deferred,
            [this, command, argv, attach_options, extra_env_vars, extra_env_varsLength, extra_keep_env, extra_keep_envLength](
                    PromiseWorker<Napi::Number, int> *worker) {
                pid_t pid;
                if (_container->attach(_container, lxc_attach_run_command, command, attach_options, &pid) < 0) {
                    worker->Error(strerror(errno));
                }
                // Cleanup: Free allocated memory
                freeCharStarArray(extra_env_vars, extra_env_varsLength);
                freeCharStarArray(extra_keep_env, extra_keep_envLength);
                free(attach_options);
                free(argv);
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



