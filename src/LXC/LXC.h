//
// Created by alexander-slaa on 11/23/23.
//

#ifndef NODE_LXC_LXC_H
#define NODE_LXC_LXC_H

#include <cerrno>
#include <sys/wait.h>

#include <lxc/lxccontainer.h>
#include <lxc/attach_options.h>
#include <lxc/version.h>

#define VERSION_AT_LEAST(major, minor, micro)                            \
    ((!(major > LXC_VERSION_MAJOR ||                    \
    major == LXC_VERSION_MAJOR && minor > LXC_VERSION_MINOR ||                \
    major == LXC_VERSION_MAJOR && minor == LXC_VERSION_MINOR && micro > LXC_VERSION_MICRO)))


#define ret_errno(__errno__)         \
    ({                           \
        errno = (__errno__); \
        -(__errno__);        \
    })

//region stuct defs
#if !VERSION_AT_LEAST(4, 0, 9) && !defined(LXC_ATTACH_SETGROUPS)
typedef struct lxc_groups_t {
    size_t size;
    gid_t *list;
} lxc_groups_t;
# endif



/* n.b. that we're just adding the fields here to shorten the definition
 * of node_lxc_migrate; in the case where we don't have the ->migrate API call,
 * we don't want to have to pass all the arguments in to let conditional
 * compilation handle things, but the call will still fail
 */
#if !VERSION_AT_LEAST(2, 0, 0)
struct migrate_opts {
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
struct lxc_console_log {
    bool clear;
    bool read;
    uint64_t *read_max;
    char *data;
};
#endif

//endregion

namespace LXC {

    static const char *get_global_config_item(const char *key) {
        return lxc_get_global_config_item(key);
    }

    static bool _lxc_has_api_extension(const char *extension) {
#if VERSION_AT_LEAST(3, 1, 0)
        return lxc_has_api_extension(extension);
#else
        return false;
#endif
    }

    static bool config_item_is_supported(const char *key) {
#if VERSION_AT_LEAST(2, 1, 0)
        return lxc_config_item_is_supported(key);
#else
        return false;
#endif
    }

    static std::string version() {
        return std::to_string(LXC_VERSION_MAJOR) + "." + std::to_string(LXC_VERSION_MINOR) + "." +
               std::to_string(LXC_VERSION_MICRO);
    }

    static int _list_all_containers(char ***names = nullptr, struct lxc_container **cret = nullptr,
                                    const char *lxc_path = nullptr) {
        if (!lxc_path) {
            lxc_path = lxc_get_global_config_item("lxc.lxcpath");
        }
        // Get the list of all containers
        return list_all_containers(lxc_path, names, &cret);
    }

    static int _list_defined_containers(char ***names = nullptr, struct lxc_container ***cret = nullptr,
                                        const char *lxc_path = nullptr) {
        if (!lxc_path) {
            lxc_path = lxc_get_global_config_item("lxc.lxcpath");
        }
        // Get the list of defined containers
        return list_all_containers(lxc_path, names, cret);
    }


    static int _list_active_containers(char ***names = nullptr, struct lxc_container ***cret = nullptr,
                                       const char *lxc_path = nullptr) {
        if (!lxc_path) {
            lxc_path = lxc_get_global_config_item("lxc.lxcpath");
        }
        // Get the list of defined containers
        return list_active_containers(lxc_path, names, cret);
    }

    class Container { //TODO Rewrite to Napi::ObjectWrap
    private:
        struct lxc_container *container;

    public:
        explicit Container(struct lxc_container *container) : container(container) {}

        ~Container() {
            if (container) {
                // Clean up the container if needed
                lxc_container_put(container);
            }
        }

        const char *state() {
            return this->container->state(this->container);
        }

        bool is_defined() {
            return this->container->is_defined(this->container);
        }

        bool is_running() {
            return this->container->is_running(this->container);
        }

        bool freeze() {
            return this->container->freeze(this->container);
        }

        bool unfreeze() {
            return this->container->unfreeze(this->container);
        }

        pid_t init_pid() {
            return this->container->init_pid(this->container);
        }


        int init_pidfd() {
#if VERSION_AT_LEAST(4, 0, 0)
            return this->container->init_pidfd(this->container);
#else
            return ret_errno(ENOSYS);
#endif
        }

        int seccomp_notify_fd() {
#if VERSION_AT_LEAST(4, 0, 0)
            return this->container->seccomp_notify_fd(this->container);
#else
            return ret_errno(ENOSYS);
#endif
        }

        int seccomp_notify_fd_active() {
#if VERSION_AT_LEAST(4, 0, 5)
            return this->container->seccomp_notify_fd_active(this->container);
#else
            return ret_errno(ENOSYS);
#endif
        }

        int set_timeout(int timeout) {
#if VERSION_AT_LEAST(5, 0, 4)
            if (!this->container->set_timeout(this->container, timeout))
        return ret_errno(EINVAL);

    return 0;
#else
            return ret_errno(ENOSYS);
#endif
        }

        int devpts_fd() {
#if VERSION_AT_LEAST(4, 0, 5)
            return this->container->devpts_fd(this->container);
#else
            return ret_errno(ENOSYS);
#endif
        }

        bool want_daemonize(bool state) {
            return this->container->want_daemonize(this->container, state);
        }

        bool want_close_all_fds(bool state) {
            return this->container->want_close_all_fds(this->container, state);
        }

        bool create(const char *t, const char *bdevtype, struct bdev_specs *specs, int flags,
                    char **const argv, size_t argc) {
            printf("template:\t %s\n", t);
            printf("bdevtype:\t %s\n", bdevtype == nullptr ? "none" : bdevtype);
            printf("flags:\t %d\n", flags);
            for (int i = 0; i < argc; ++i) {
                printf("[%d]:\t %s\n", i, argv[i]);
            }
            auto flag = !!(flags & LXC_CREATE_QUIET);
            if (strncmp(t, "none", strlen(t)) == 0) {
                printf("using none %d\n", flag);
                return this->container->create(this->container, nullptr, bdevtype, specs, flag,
                                               argv);
//                return this->container->create(this->container, nullptr, bdevtype, specs, flags,
//                                               argv);
            }
            printf("using defined %d\n", flag);
            return this->container->create(this->container, t, bdevtype, specs, flag, argv);
//            return this->container->create(this->container, t, bdevtype, specs, flags, argv);
        }

        bool start(int useinit, char *const argv[]) {
            return this->container->start(this->container, useinit, argv);
        }

        bool stop() {
            return this->container->stop(this->container);
        }

        bool reboot() {
            return this->container->reboot(this->container);
        }

        bool shutdown(int timeout) {
            return this->container->shutdown(this->container, timeout);
        }


        char *config_file_name() {
            return this->container->config_file_name(this->container);
        }

        bool destroy() {
            return this->container->destroy(this->container);
        }

        bool destroy_with_snapshots() {
#if VERSION_AT_LEAST(1, 1, 0)
            return this->container->destroy_with_snapshots(this->container);
#else
            return false;
#endif
        }

        bool wait(const char *state, int timeout) {
            return this->container->wait(this->container, state, timeout);
        }

        char *get_config_item(const char *key) {
            char *value;

            int len = this->container->get_config_item(this->container, key, nullptr, 0);
            if (len <= 0)
                return nullptr;

            again:
            value = (char *) malloc(sizeof(char) * len + 1);
            if (value == nullptr)
                goto again;

            if (this->container->get_config_item(this->container, key, value, len + 1) != len) {
                free(value);
                return nullptr;
            }

            return value;
        }

        bool set_config_item(const char *key, const char *value) {
            return this->container->set_config_item(this->container, key, value);
        }

        void clear_config() {
            this->container->clear_config(this->container);
        }

        bool clear_config_item(const char *key) {
            return this->container->clear_config_item(this->container, key);
        }

        char *get_running_config_item(const char *key) {
            return this->container->get_running_config_item(this->container, key);
        }

        char *get_keys(const char *key) {
            char *value = NULL;

            int len = this->container->get_keys(this->container, key, NULL, 0);
            if (len <= 0)
                return NULL;

            again:
            value = (char *) malloc(sizeof(char) * len + 1);
            if (value == NULL)
                goto again;

            if (this->container->get_keys(this->container, key, value, len + 1) != len) {
                free(value);
                return NULL;
            }

            return value;
        }

        char *get_cgroup_item(const char *key) {
            char *value = NULL;

            int len = this->container->get_cgroup_item(this->container, key, NULL, 0);
            if (len <= 0)
                return NULL;

            again:
            value = (char *) malloc(sizeof(char) * len + 1);
            if (value == NULL)
                goto again;

            if (this->container->get_cgroup_item(this->container, key, value, len + 1) != len) {
                free(value);
                return NULL;
            }

            return value;
        }

        bool set_cgroup_item(const char *key, const char *value) {
            return this->container->set_cgroup_item(this->container, key, value);
        }

        const char *get_config_path() {
            return this->container->get_config_path(this->container);
        }

        bool set_config_path(const char *path) {
            return this->container->set_config_path(this->container, path);
        }

        bool load_config(const char *alt_file) {
            return this->container->load_config(this->container, alt_file);
        }

        bool save_config(const char *alt_file) {
            return this->container->save_config(this->container, alt_file);
        }

        bool clone(const char *newname, const char *lxcpath, int flags, const char *bdevtype) {
            auto c2 = this->container->clone(this->container, newname, lxcpath, flags, bdevtype, NULL, 0, NULL);
            if (c2 == nullptr) {
                return false;
            }
            lxc_container_put(c2);
            return true;
        }

        int console_getfd(int ttynum) {
            int mainfd;
            int ret;
            ret = this->container->console_getfd(this->container, &ttynum, &mainfd);
            if (ret < 0)
                return ret;

            return mainfd;
        }

        bool console(int ttynum, int stdinfd, int stdoutfd, int stderrfd, int escape) {
            if (this->container->console(this->container, ttynum, stdinfd, stdoutfd, stderrfd, escape) == 0) {
                return true;
            }
            return false;
        }

        char **get_interfaces() {
            return this->container->get_interfaces(this->container);
        }

        char **get_ips(const char *interface, const char *family, int scope) {
            return this->container->get_ips(this->container, interface, family, scope);
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

        int attach_no_wait(
                bool clear_env,
                int namespaces,
                long personality,
                uid_t uid, gid_t gid, lxc_groups_t groups,
                int stdinfd, int stdoutfd, int stderrfd,
                char *initial_cwd,
                char **extra_env_vars,
                char **extra_keep_env,
                const char *const argv[],
                pid_t *attached_pid,
                int attach_flags) {
            int ret;

            lxc_attach_options_t attach_options = LXC_ATTACH_OPTIONS_DEFAULT;
            attach_options.attach_flags = attach_flags;

            lxc_attach_command_t command = (lxc_attach_command_t) {.program = NULL, .argv = NULL};

            attach_options.env_policy = LXC_ATTACH_KEEP_ENV;
            if (clear_env) {
                attach_options.env_policy = LXC_ATTACH_CLEAR_ENV;
            }

            attach_options.namespaces = namespaces;
            attach_options.personality = personality;

            attach_options.uid = uid;
            attach_options.gid = gid;
#if VERSION_AT_LEAST(4, 0, 9)
            if (groups.size > 0) {
                attach_options.groups = groups;
                attach_options.attach_flags &= LXC_ATTACH_SETGROUPS;
            }
#endif

            attach_options.stdin_fd = stdinfd;
            attach_options.stdout_fd = stdoutfd;
            attach_options.stderr_fd = stderrfd;

            attach_options.initial_cwd = initial_cwd;
            attach_options.extra_env_vars = extra_env_vars;
            attach_options.extra_keep_env = extra_keep_env;

            command.program = (char *)
                    argv[0];
            command.argv = (char **) argv;
            ret = this->container->attach(this->container, lxc_attach_run_command, &command, &attach_options,
                                          attached_pid);
            if (ret < 0)
                return ret;

            return 0;
        }

        int attach(
                bool clear_env,
                int namespaces,
                long personality,
                uid_t uid, gid_t gid, lxc_groups_t groups,
                int stdinfd, int stdoutfd, int stderrfd,
                char *initial_cwd,
                char **extra_env_vars,
                char **extra_keep_env,
                int attach_flags) {
            int ret;
            pid_t pid;

            lxc_attach_options_t attach_options = LXC_ATTACH_OPTIONS_DEFAULT;
            attach_options.attach_flags = attach_flags;

            attach_options.env_policy = LXC_ATTACH_KEEP_ENV;
            if (clear_env) {
                attach_options.env_policy = LXC_ATTACH_CLEAR_ENV;
            }

            attach_options.namespaces = namespaces;
            attach_options.personality = personality;

            attach_options.uid = uid;
            attach_options.gid = gid;
#if VERSION_AT_LEAST(4, 0, 9)
            if (groups.size > 0) {
                attach_options.groups = groups;
                attach_options.attach_flags &= LXC_ATTACH_SETGROUPS;
            }
#endif

            attach_options.stdin_fd = stdinfd;
            attach_options.stdout_fd = stdoutfd;
            attach_options.stderr_fd = stderrfd;

            attach_options.initial_cwd = initial_cwd;
            attach_options.extra_env_vars = extra_env_vars;
            attach_options.extra_keep_env = extra_keep_env;

            ret = this->container->attach(this->container, lxc_attach_run_shell, NULL, &attach_options, &pid);
            if (ret < 0)
                return ret;

            ret = wait_for_pid_status(pid);
            if (ret < 0)
                return ret;

            if (WIFEXITED(ret))
                return WEXITSTATUS(ret);

            return ret;
        }

        int attach_run_wait(
                bool clear_env,
                int namespaces,
                long personality,
                uid_t uid, gid_t gid, lxc_groups_t groups,
                int stdinfd, int stdoutfd, int stderrfd,
                char *initial_cwd,
                char **extra_env_vars,
                char **extra_keep_env,
                const char *const argv[],
                int attach_flags) {
            int ret;

            lxc_attach_options_t attach_options = LXC_ATTACH_OPTIONS_DEFAULT;
            attach_options.attach_flags = attach_flags;

            attach_options.env_policy = LXC_ATTACH_KEEP_ENV;
            if (clear_env) {
                attach_options.env_policy = LXC_ATTACH_CLEAR_ENV;
            }

            attach_options.namespaces = namespaces;
            attach_options.personality = personality;

            attach_options.uid = uid;
            attach_options.gid = gid;
#if VERSION_AT_LEAST(4, 0, 9)
            if (groups.size > 0) {
                attach_options.groups = groups;
                attach_options.attach_flags &= LXC_ATTACH_SETGROUPS;
            }
#endif

            attach_options.stdin_fd = stdinfd;
            attach_options.stdout_fd = stdoutfd;
            attach_options.stderr_fd = stderrfd;

            attach_options.initial_cwd = initial_cwd;
            attach_options.extra_env_vars = extra_env_vars;
            attach_options.extra_keep_env = extra_keep_env;

            ret = this->container->attach_run_wait(this->container, &attach_options, argv[0], argv);
            if (WIFEXITED(ret) && WEXITSTATUS(ret) == 255)
                return -1;
            return ret;
        }

        bool may_control() {
            return this->container->may_control(this->container);
        }

        int snapshot() {
            return this->container->snapshot(this->container, NULL);
        }

        int snapshot_list(struct lxc_snapshot **ret) {
            return this->container->snapshot_list(this->container, ret);
        }

        bool snapshot_restore(const char *snapname, const char *newname) {
            return this->container->snapshot_restore(this->container, snapname, newname);
        }

        bool snapshot_destroy(const char *snapname) {
            return this->container->snapshot_destroy(this->container, snapname);
        }

        bool snapshot_destroy_all() {
#if VERSION_AT_LEAST(1, 1, 0)
            return this->container->snapshot_destroy_all(this->container);
#else
            return false;
#endif
        }

        bool add_device_node(const char *src_path, const char *dest_path) {
            return this->container->add_device_node(this->container, src_path, dest_path);
        }

        bool remove_device_node(const char *src_path, const char *dest_path) {
            return this->container->remove_device_node(this->container, src_path, dest_path);
        }

        bool rename(const char *newname) {
            return this->container->rename(this->container, newname);
        }

        bool checkpoint(char *directory, bool stop, bool verbose) {
#if VERSION_AT_LEAST(1, 1, 0)
            return this->container->checkpoint(this->container, directory, stop, verbose);
#else
            return false;
#endif
        }

        bool restore(char *directory, bool verbose) {
#if VERSION_AT_LEAST(1, 1, 0)
            return this->container->restore(this->container, directory, verbose);
#else
            return false;
#endif
        }

        int migrate(unsigned int cmd, struct migrate_opts *opts) {
#if VERSION_AT_LEAST(2, 0, 0)
            return this->container->migrate(this->container, cmd, opts, sizeof(*opts));
#else
            return -EINVAL;
#endif
        }

        bool attach_interface(const char *dev, const char *dst_dev) {
#if VERSION_AT_LEAST(1, 1, 0)
            return this->container->attach_interface(this->container, dev, dst_dev);
#else
            return false;
#endif
        }

        bool detach_interface(const char *dev, const char *dst_dev) {
#if VERSION_AT_LEAST(1, 1, 0)
            return this->container->detach_interface(this->container, dev, dst_dev);
#else
            return false;
#endif
        }


        int error_num() {
            return this->container->error_num;
        }

        char * error_str() {
            return this->container->error_string;
        }

        int console_log(struct lxc_console_log *log) {
#if VERSION_AT_LEAST(3, 0, 0)
            return this->container->console_log(this->container, log);
#else
            return false;
#endif
        }


        static Container *New(Napi::Env env, const char *name, const char *configpath) {
            // Create the container
            struct lxc_container *container = lxc_container_new(name, configpath);
            if (!container) {
                // Handle error (e.g., container creation failed)
                Napi::TypeError::New(env, "Failed to create container").ThrowAsJavaScriptException();
            }
            return new Container(container);
        }
    };


};


#endif //NODE_LXC_LXC_H
