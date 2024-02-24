//region consts
import {lxc_attach_options} from "./types/lxc_attach_options";

export enum LXC_LOGLEVEL {
    TRACE = "TRACE",
    DEBUG = "DEBUG",
    INFO = "INFO",
    NOTICE = "NOTICE",
    WARN = "WARN",
    ERROR = "ERROR",
    CRIT = "CRIT",
    ALERT = "ALERT",
    FATAL = "FATAL",
}

export enum LXC_CREATE {
    QUIET = (1 << 0),
    MAXFLAGS = (1 << 1),
}

export enum LXC_CLONE {
    /**
     * Do not edit the rootfs to change the hostname
     */
    KEEPNAME = (1 << 0),
    /**
     * Do not change the MAC address on network interfaces
     */
    KEEPMACADDR = (1 << 1),
    /**
     * Snapshot the original filesystem(s)
     */
    SNAPSHOT = (1 << 2),
    /**
     * Use the same bdev type
     */
    KEEPBDEVTYPE = (1 << 3),
    /**
     * Snapshot only if bdev supports it, else copy
     */
    MAYBE_SNAPSHOT = (1 << 4),
    /**
     * Number of `LXC_CLONE_*` flags
     */
    MAXFLAGS = (1 << 5),
    /**
     * allow snapshot creation even if source container is running
     */
    ALLOW_RUNNING = (1 << 6)
}

export enum LXC_ATTACH {
    /**
     * Move to cgroup
     * @default on
     */
    MOVE_TO_CGROUP = 0x00000001,
    /**
     * Drop capabilities
     * @default on
     */
    DROP_CAPABILITIES = 0x00000002,

    /**
     * Set personality
     * @default on
     */
    SET_PERSONALITY = 0x00000004,

    /**
     * Execute under a Linux Security Module
     * @default on
     */
    LSM_EXEC = 0x00000008,

    /**
     * Remount /proc filesystem
     * @default off
     */
    REMOUNT_PROC_SYS = 0x00010000,

    /**
     * TODO: currently unused
     * @default off
     */
    LSM_NOW = 0x00020000,

    /**
     * PR_SET_NO_NEW_PRIVS
     * Set PR_SET_NO_NEW_PRIVS to block execve() gainable privileges.
     *
     * @default off
     *
     */
    NO_NEW_PRIVS = 0x00040000,

    /**
     * Allocate new terminal for attached process.
     * @default off
     */
    TERMINAL = 0x00080000,

    /**
     * Set custom LSM label specified in @lsm_label.
     * @default off
     */
    LSM_LABEL = 0x00100000,

    /**
     * Set additional group ids specified in @groups.
     * @default off
     */
    SETGROUPS = 0x00200000,

    /**
     * We have 16 bits for things that are on by default and 16 bits that
     * are off by default, that should be sufficient to keep binary
     * compatibility for a while
     *
     * Mask of flags to apply by default
     */
    DEFAULT = 0x0000FFFF
}

export enum LXC_MIGRATE{
    MIGRATE_PRE_DUMP = 0,
    MIGRATE_DUMP = 1,
    MIGRATE_RESTORE = 2,
    MIGRATE_FEATURE_CHECK = 3,
}
export enum LXC_MOUNT{
    V1 = 0
}

export const DEFAULT_ATTACH: lxc_attach_options = {
    namespaces: -1,
    personality: -1,
    cwd: "/",
    uid: -1,
    gid: -1,
    groups: [],
    clear_env: false,
    extra_env_vars: [],
    stdio: [process.stdin.fd, process.stdout.fd, process.stderr.fd],
    remount_sys_proc: false,
    elevated_privileges: false,
    log_fd: -1,
    lsm_label: undefined
}

//endregion