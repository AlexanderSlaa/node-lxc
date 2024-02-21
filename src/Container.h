//
// Created by alexanderslaa on 12/16/23.
//

#ifndef NODE_LXC_CONTAINER_H
#define NODE_LXC_CONTAINER_H

#include <napi.h>
#include <lxc/lxccontainer.h>
#include <lxc/version.h>

#define VERSION_AT_LEAST(major, minor, micro)                      \
    ((!(major > LXC_VERSION_MAJOR ||                               \
        major == LXC_VERSION_MAJOR && minor > LXC_VERSION_MINOR || \
        major == LXC_VERSION_MAJOR && minor > LXC_VERSION_MINOR || \
        major == LXC_VERSION_MAJOR && minor == LXC_VERSION_MINOR && micro > LXC_VERSION_MICRO)))

#define ret_errno(__errno__) \
    ({                       \
        errno = (__errno__); \
        -(__errno__);        \
    })

// region stuct defs
#if !VERSION_AT_LEAST(4, 0, 9) && !defined(LXC_ATTACH_SETGROUPS)
typedef struct lxc_groups_t
{
    size_t size;
    gid_t *list;
} lxc_groups_t;
#endif

/* n.b. that we're just adding the fields here to shorten the definition
 * of node_lxc_migrate; in the case where we don't have the ->migrate API call,
 * we don't want to have to pass all the arguments in to let conditional
 * compilation handle things, but the call will still fail
 */
#if !VERSION_AT_LEAST(2, 0, 0)
struct migrate_opts
{
    char *directory;
    bool verbose;
    bool stop;
    char *predump_dir;
};
#endif

/* This is a struct that we can add "extra" (i.e. options added after 2.0.0)
 * migrate options to, so that we don't have to have a massive function
 * signature when the list of options grows.
 */
struct extra_migrate_opts {
    bool preserves_inodes;
    char *action_script;
    uint64_t ghost_limit;
    uint64_t features_to_check;
};

#if !VERSION_AT_LEAST(3, 0, 0)
struct lxc_console_log
{
    bool clear;
    bool read;
    uint64_t *read_max;
    char *data;
};
#endif

class Container : public Napi::ObjectWrap<Container> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);

    static Napi::Object New(Napi::Env env, const std::initializer_list<napi_value> &args);

    explicit Container(const Napi::CallbackInfo &info);

    ~Container() override;

private:
    Napi::Value GetName(const Napi::CallbackInfo &info);
    void SetName(const Napi::CallbackInfo &info, const Napi::Value &value);

    Napi::Value GetState(const Napi::CallbackInfo &info);
    Napi::Value GetRunning(const Napi::CallbackInfo &info);
    Napi::Value GetInitPID(const Napi::CallbackInfo &info);
    Napi::Value GetConfigFileName(const Napi::CallbackInfo &info);

    Napi::Value GetDaemonize(const Napi::CallbackInfo &info);
    void SetDaemonize(const Napi::CallbackInfo &info, const Napi::Value &value);


    Napi::Value GetDefined(const Napi::CallbackInfo &info);

    Napi::Value Freeze(const Napi::CallbackInfo &info);
    Napi::Value Unfreeze(const Napi::CallbackInfo &info);
    Napi::Value LoadConfig(const Napi::CallbackInfo &info);


    Napi::Value Start(const Napi::CallbackInfo &info);
    Napi::Value Stop(const Napi::CallbackInfo &info);


    Napi::Value CloseAllFds(const Napi::CallbackInfo &info);
    Napi::Value Wait(const Napi::CallbackInfo &info);
    void SetConfigItem(const Napi::CallbackInfo &info);
    Napi::Value Destroy(const Napi::CallbackInfo &info);
    Napi::Value Save(const Napi::CallbackInfo &info);

    Napi::Value Create(const Napi::CallbackInfo &info);
    Napi::Value Reboot(const Napi::CallbackInfo &info);
    Napi::Value Shutdown(const Napi::CallbackInfo &info);

    void ClearConfig(const Napi::CallbackInfo &info);
    void ClearConfigItem(const Napi::CallbackInfo &info);
    Napi::Value GetConfigItem(const Napi::CallbackInfo &info);
    Napi::Value GetRunningConfigItem(const Napi::CallbackInfo &info);
    Napi::Value GetKeys(const Napi::CallbackInfo &info);
    Napi::Value GetInterfaces(const Napi::CallbackInfo &info);
    Napi::Value GetIPs(const Napi::CallbackInfo &info);

    Napi::Value GetCGroupItem(const Napi::CallbackInfo &info);
    void SetCGroupItem(const Napi::CallbackInfo &info);

    Napi::Value Clone(const Napi::CallbackInfo &info);


    // Attach with wait for process lxc_attach_run_shell
    Napi::Value Attach(const Napi::CallbackInfo &info);
    // Attach with no wait lxc_attach_run_command
    Napi::Value Exec(const Napi::CallbackInfo &info);
    Napi::Value Console(const Napi::CallbackInfo &info);


    struct lxc_container *_container;
};

#endif // NODE_LXC_CONTAINER_H
