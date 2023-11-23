import {Container} from "./types/container";

export const binding = <LXCBinding>require('../build/Release/node-lxc');

export const macros = {
    /** VersionAtLeast returns true when the tested version >= current version.
     *
     * @param major
     * @param minor
     * @param micro
     * @constructor
     */
    VersionAtLeast(major: number, minor: number, micro: number) {
        if (binding.version.devel) {
            return true
        }
        const versionInfo = binding.version;
        if (major > versionInfo.major) {
            return false
        }
        if (major == versionInfo.major && minor > versionInfo.minor) {
            return false
        }
        if (major == versionInfo.major && minor == versionInfo.minor && micro > versionInfo.micro) {
            return false
        }
        return true
    },
}

export const consts = {
    LXC_MOUNT_API_V1: 1,
}

export enum LXC_CLONE {
    CLONE_KEEPNAME = (1 << 0),
    CLONE_KEEPMACADDR = (1 << 1),
    CLONE_SNAPSHOT = (1 << 2),
    CLONE_KEEPBDEVTYPE = (1 << 3),
    CLONE_MAYBE_SNAPSHOT = (1 << 4),
    CLONE_MAXFLAGS = (1 << 5),
    CLONE_ALLOW_RUNNING = (1 << 6),
}

export enum LXC_CREATE {
    QUIET = (1 << 0),
    MAXFLAGS = (1 << 1),
}

export enum LXC_MIGRATE {
    PRE_DUMP = 0,
    DUMP = 1,
    RESTORE = 2,
    FEATURE_CHECK = 3,
}

/**
 * Specifications for how to create a new backing store
 */
export type bdev_specs = {
    fstype: string; /*!< Filesystem type */
    fssize: number;  /*!< Filesystem size in bytes */
    zfs: {
        zfsroot: string
    }
    lvm: {
        vg: string,
        lv: string,
        thinpool: string,
    }
    dir: string; /*!< Directory path */
    rbd: {
        rbdname: string,
        rbdpool: string,
    }
}

export type migrate_opts = {
    /* new members should be added at the end */
    directory: string;
    verbose: boolean;

    stop: boolean; /* stop the container after dump? */
    predump_dir: string; /* relative to directory above */
    pageserver_address: string; /* where should memory pages be send? */
    pageserver_port: string;

    /**
     *  This flag indicates whether or not the container's rootfs will have
     * the same inodes on checkpoint and restore. In the case of e.g. zfs
     * send or btrfs send, or an LVM snapshot, this will be true, but it
     * won't if e.g. you rsync the filesystems between two machines.
     */
    preserves_inodes: boolean;

    /* Path to an executable script that will be registered as a criu
     * "action script"
     */
    action_script: string;

    /**
     * If CRIU >= 2.4 is detected the option to skip in-flight connections
     * will be enabled by default. The flag 'disable_skip_in_flight' will
     * unconditionally disable this feature. In-flight connections are
     * not fully established TCP connections: SYN, SYN-ACK */
    disable_skip_in_flight: boolean;

    /**
     * This is the maximum file size for deleted files (which CRIU calls
     * "ghost" files) that will be handled. 0 indicates the CRIU default,
     * which at this time is 1MB.
     */
    ghost_limit: number; //uint64_t

    /**
     * Some features cannot be checked by comparing the CRIU version.
     * Features like dirty page tracking or userfaultfd depend on
     * the architecture/kernel/criu combination. This is a bitmask
     * in which the desired feature checks can be encoded.
     */
    features_to_check: number; //uint64_t
}

type LXCBinding = {
    /**
     * Version object form LXC C api (constants).
     */
    version: { major: number, minor: number, micro: number, devel: 0 | 1, abi: string };
    /**
     * GlobalConfigItem returns the value of the given global config key.
     * @param key
     * @constructor
     */
    lxc_get_global_config_item(key: string): string | null;
    /**
     * ContainerNames returns the names of defined and active containers on the system.
     * @param lxc_path
     * @constructor
     */
    list_all_containers(lxc_path?: string): string[];
    /**
     * DefinedContainerNames returns the names of the defined containers on the system.
     * @param lxc_path
     * @constructor
     */
    list_defined_containers(lxc_path?: string): string[];
    /**
     * ActiveContainerNames returns the names of the active containers on the system.
     * @param lxc_path
     * @constructor
     */
    list_active_containers(lxc_path?: string): string[];
    /**
     * NewContainer returns a new container external reference.
     * @param name
     * @param configPath
     */
    lxc_container_new(name: string, configPath?: string): {};
    /**
     * HasAPIExtension returns true if the extension is supported.
     * @param extension
     * @constructor
     */
    lxc_has_api_extension(extension: string): boolean;
    /**
     * IsSupportedConfigItem returns true if the key belongs to a supported config item.
     * @param key
     * @constructor
     */
    lxc_config_item_is_supported(key: string): boolean;
    /**
     * Determine if /var/lib/lxc/$name/config exists.
     * @return true if container is defined, else false.
     * @param container
     */
    lxc_defined(container: Container): boolean
    /**
     * Determine state of container.
     * @return string Static upper-case string representing state of container.
     * @param container
     */
    lxc_state(container: Container): string
    /**
     * Determine if container is running.
     * @return true on success, else false.
     */
    lxc_running(container: Container): boolean
    /**
     * Freeze running container.
     * @param container
     */
    lxc_freeze(container: Container): boolean
    /**
     * Unfreeze running container.
     * @param container
     */
    lxc_unfreeze(container: Container): boolean
    /**
     * Determine process ID of the containers init process.
     * @param container
     */
    lxc_init_pid(container: Container): number
    /**
     * Retrieve a pidfd for the container's init process.
     * @param container
     */
    lxc_init_pidfd(container: Container): number
    /**
     * Retrieve a file descriptor for the container's seccomp filter.
     * @param container
     */
    lxc_seccomp_notify_fd(container: Container): number
    /**
     * Retrieve a file descriptor for the running container's seccomp filter.
     * @param container
     */
    lxc_seccomp_notify_fd_active(container: Container): number
    /**
     * @param container
     */
    lxc_set_timeout(container: Container): number //TODO dev not correct version
    /**
     * Retrieve a mount fd for the container's devpts instance.
     * @param container
     */
    lxc_devpts_fd(container: Container): number
    /**
     * Change whether the container wants to run disconnected from the terminal.
     * @param container
     * @param state Value for the daemonize bit (true(1) or false(0))
     */
    lxc_want_daemonize(container: Container, state: boolean): boolean
    /**
     * Change whether the container wishes all file descriptors to be closed on startup.
     * The LISTEN_FDS environment variable can be set to keep inherited file descriptors open.
     * @param container
     * @param state Value for the daemonize bit (true(1) or false(0))
     */
    lxc_want_close_all_fds(container: Container, state: boolean): boolean
    /**
     * @param container
     * @param template Template to execute to instantiate the root filesystem and adjust the configuration.
     * @param bdevtype Backing store type to use (if NULL, dir will be used).
     * @param bdevspecs Additional parameters for the backing store (for example LVM volume group to use).
     * @param flags LXC_CREATE_* options (currently only LXC_CREATE_QUIET is supported).
     * @param argsv
     */
    lxc_create(container: Container, template: string | "none", bdevtype: string | null, bdevspecs?: bdev_specs, flags?: number, argsv?: string[]): Container //TODO remake function C!!!
    /**
     * Start the container.
     * @param container
     * @param useinit Use lxcinit rather than /sbin/init.
     * @param argv Array of arguments to pass to init.
     */
    lxc_start(container: Container, useinit: number, ...argv: string[]): boolean
    /**
     * Stop the container.
     * @param container
     */
    lxc_stop(container: Container): boolean
    /**
     * Request the container reboot by sending it SIGINT.
     * @param container
     */
    lxc_reboot(container: Container): boolean
    /**
     * Request the container shutdown by sending it SIGPWR.
     * @param container
     * @param timeout Seconds to wait before returning false. (-1 to wait forever, 0 to avoid waiting)
     */
    lxc_shutdown(container: Container, timeout: number): boolean
    /**
     * Return current config file name.
     * @param container
     */
    lxc_config_file_name(container: Container): string
    /**
     * Delete the container.
     * @param container
     */
    lxc_destroy(container: Container): boolean
    /**
     * Delete the container and all its snapshots.
     * @param container
     */
    lxc_destroy_with_snapshots(container: Container): boolean
    /**
     * Wait for container to reach a particular state.
     * @param container
     * @param state State to wait for.
     * @param timeout Timeout in seconds.
     *
     * @note
     * A timeout of -1 means wait forever. A timeout of 0 means do not wait.
     * @return boolean `true` if state reached within timeout, else `false`.
     */
    lxc_wait(container: Container, state: string, timeout: number): boolean
    /**
     * Retrieve the value of a config item.
     * @param container
     * @param key Name of option to get
     */
    lxc_get_config_item(container: Container, key: string): string
    /**
     * Set a key/value configuration option.
     * @param container
     * @param key Name of option to set.
     * @param value Value of name to set.
     *
     * @return boolean true on success, else false.
     */
    lxc_set_config_item(container: Container, key: string, value: string): boolean
    /**
     * Completely clear the containers in-memory configuration.
     * @param container
     */
    lxc_clear_config(container: Container): void
    /**
     * Clear a configuration item.
     * @param container
     * @param key Name of option to clear.
     * @note
     * Analog of set_config_item
     */
    lxc_clear_config_item(container: Container, key: string): boolean
    /**
     * Retrieve the value of a config item from running container.
     * @param container
     * @param key Name of option to get.
     */
    lxc_get_running_config_item(container: Container, key: string): string
    /**
     * Retrieve a list of config item keys given a key prefix
     * @param container
     * @param key Name of option to get.
     */
    lxc_get_keys(container: Container, key: string): string //TODO not correct with docs linuxcontainer.h:418
    /**
     * Retrieve the specified cgroup subsystem value for the container.
     * @param container
     * @param subsys cgroup subsystem to retrieve.
     */
    lxc_get_cgroup_item(container: Container, subsys: string): string //TODO not correct with implemented linuxcontainer.h:469
    /**
     * Set the specified cgroup subsystem value for the container.
     * @param container
     * @param subsys cgroup subsystem to consider.
     * @param value Value to set for subsys.
     */
    lxc_set_cgroup_item(container: Container, subsys: string, value: string): boolean
    /**
     * Determine full path to the containers configuration file.
     * Each container can have a custom configuration path.
     * However, by default it will be set to either the LXCPATH configure variable, or the lxcpath value in the LXC_GLOBAL_CONF configuration file (i.e. /etc/lxc/lxc.conf).
     * The value for a specific container can be changed using
     *
     * @see set_config_path. There is no other way to specify this in general at the moment.
     *
     * @param container
     * @return string representing full path to configuration file.
     */
    lxc_get_config_path(container: Container): string
    /**
     * Set the full path to the containers configuration file.
     * @param container
     * @param path Full path to configuration file.
     */
    lxc_set_config_path(container: Container, path: string): boolean
    /**
     * Load the specified configuration for the container.
     * @param container
     * @param alt_file Full path to alternate configuration file, or undefined to use the default configuration file.
     */
    lxc_load_config(container: Container, alt_file?: string): boolean
    /**
     * Save configuration to a file.
     * @param container
     * @param alt_file Full path to file to save configuration in.
     */
    lxc_save_config(container: Container, alt_file: string): boolean
    /**
     *
     * @param container
     * @param new_name New name for the container. If undefined, the same name is used and a new lxcpath MUST be specified.
     * @param lxc_path lxcpath in which to create the new container. If undefined, the original container's lxcpath will be used. (XXX: should we use the default instead?)
     * @param flags LXC_CREATE
     * @param bdevtype Optionally force the cloned bdevtype to a specified plugin. By default, the original is used (subject to snapshot requirements).
     *
     * @param bdevdata Information about how to create the new storage (i.e. fstype and fsdata).
     *
     * @param newsize In case of a block device backing store, an optional size. If 0, the original backing store's size will be used if possible. Note this only applies to the rootfs. For any other filesystems, the original size will be duplicated.
     * @param hookargs Additional arguments to pass to the clone hook script.
     *
     * @return Newly-allocated copy of container c
     *
     * @note
     *
     * If devtype was not specified, and flags contains
     * @see LXC_CLONE_SNAPSHOT then use the native bdevtype if possible, else use an overlayfs.
     */
    lxc_clone(container: Container, new_name: string | undefined, lxc_path: string | undefined, flags: number, bdevtype: string, bdevdata: string, newsize: number, hookargs: string[]): unknown //TODO incomplete C bindings
    /**
     * Allocate a console tty for the container.
     * @param container
     * @param ttynum Terminal number to attempt to allocate, or -1 to allocate the first available tty.
     * @return tty file descriptor number on success, or -1 on failure.
     *
     * @note
     * On successful return, ttynum will contain the tty number that was allocated.
     *
     * The returned file descriptor is used to keep the tty allocated.
     * The caller should call close(2) on the returned file descriptor when no longer required so that it may be allocated by another caller.
     *
     */
    lxc_console_getfd(container: Container, ttynum: number): number
    /**
     * Allocate and run a console tty.
     * @param container
     * @param ttynum Terminal number to attempt to allocate, -1 to allocate the first available tty or 0 to allocate the console.
     * @param stdinfd File descriptor to read input from.
     * @param stdoutfd File descriptor to write output to.
     * @param stderrfd File descriptor to write error output to.
     * @param escape The escape character (1 == 'a', 2 == 'b', ...).
     */
    lxc_console(container: Container, ttynum: number, stdinfd: number, stdoutfd: number, stderrfd: number, escape: number): boolean
    /**
     * Obtain a list of network interfaces.
     * @param container
     * @return array of network interfaces
     */
    lxc_get_interfaces(container: Container): string[]
    /**
     * Determine the list of container IP addresses.
     * @param container
     */
    lxc_get_ips(container: Container): string[]
    /**
     * Add specified device to the container.
     * @param container
     * @param src_path Full path of the device.
     * @param dest_path Alternate path in the container (or undefined to use src_path).
     */
    lxc_add_device_node(container: Container, src_path: string, dest_path?: string): boolean
    /**
     * Remove specified device from the container.
     * @param container
     * @param src_path Full path of the device.
     * @param dest_path Alternate path in the container (or undefined to use src_path).
     */
    lxc_remove_device_node(container: Container, src_path: string, dest_path?: string): boolean
    /**
     * Rename a container
     * @param container
     * @param newname New name to be used for the container.
     */
    lxc_rename(container: Container, newname: string): boolean
    /**
     *
     * @param container
     * @param directory The directory to dump the container to.
     * @param stop Whether or not to stop the container after checkpointing.
     * @param verbose Enable criu's verbose logs.
     *
     * @return true on success, else false. present at compile time.
     */
    lxc_checkpoint(container: Container, directory: string, stop: boolean, verbose: boolean): boolean
    /**
     *
     * @param container
     * @param directory The directory to restore the container from.
     * @param verbose Enable criu's verbose logs.
     */
    lxc_restore(container: Container, directory: string, verbose: boolean): boolean
    /**
     *
     * @param container
     * @param cmd One of the LXC_MIGRATE constants.
     * @param opts A migrate_opts struct filled with relevant options.
     */
    lxc_migrate(container: Container, cmd: LXC_MIGRATE, opts: migrate_opts): number
    /**
     * Add specified netdev to the container.
     * @param container
     * @param dev name of net device.
     * @param dst_dev destination name of net device.
     */
    lxc_attach_interface(container: Container, dev: string, dst_dev: string): boolean
    /**
     * Remove specified netdev from the container.
     * @param container
     * @param dev name of net device.
     * @param dst_dev destination name of net device.
     */
    lxc_detach_interface(container: Container, dev: string, dst_dev: string): boolean
    /**
     * Last error number
     * @param container
     */
    lxc_error_num(container: Container): number
}