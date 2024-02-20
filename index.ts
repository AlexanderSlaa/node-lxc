let bindings
if (process.env["MODE"] === "DEV") {
    console.warn("!!!!RUNNING IN DEV MODE!!!!");
    bindings = require("./build/Debug/node-lxc.node");
} else {
    bindings = require("./build/Release/node-lxc.node");

}

//region consts
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


export const DEFAULT_ATTACH: ContainerAttachOptions = {
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
    elevated_privileges: false
}

//endregion

//region types

export enum Personality {
    /**
     * Intel 32bit
     */
    X86 = 0x0008,
    /**
     * Intel 64bit
     */
    X86_64 = 0x0000
}

export type ContainerAttachOptions = ({
    /**
     * If ClearEnv is true the environment is cleared before running the command.
     */
    clear_env: true,
    /**
     * EnvToKeep specifies the environment of the process when ClearEnv is true.
     */
    extra_keep_env: string[]
} |
    {
        /**
         * If ClearEnv is true the environment is cleared before running the command.
         */
        clear_env: false
    }) &
    {
        /**
         * Specify the namespaces to attach to, as OR'ed list of clone flags (syscall.CLONE_NEWNS | syscall.CLONE_NEWUTS ...).
         */
        namespaces: number,
        /**
         * Specify the architecture which the kernel should appear to be running as to the command executed.
         */
        personality: Personality | -1,
        /**
         * UID specifies the user id to run as.
         */
        uid: number,
        /**
         * GID specifies the group id to run as.
         */
        gid: number,
        /**
         * Groups specifies the list of additional group ids to run with.
         */
        groups: number[],
        /**
         * [stdinfd, stdoutfd, stderrfd]
         * stdinfd: specifies the fd to read input from.
         * stdoutdf: specifies the fd to write output to.
         * stderrfd: specifies the fd to write error output to.
         */
        stdio: [number, number, number],
        /**
         * Cwd specifies the working directory of the command.
         */
        cwd: string,
        /**
         * Env specifies the environment of the process.
         */
        extra_env_vars: string[],
        /**
         * RemountSysProc remounts /sys and /proc for the executed command.
         * This is required to reflect the container (PID) namespace context
         * if the command does not attach to the container's mount namespace.
         */
        remount_sys_proc: boolean
        /**
         * ElevatedPrivileges runs the command with elevated privileges.
         * The capabilities, cgroup and security module restrictions of the container are not applied.
         * @WARNING: This may leak privileges into the container.
         */
        elevated_privileges: boolean
    }

export type bdev_specs = {
    /**
     * Filesystem type
     */
    fstype: string;
    /**
     * Filesystem size in bytes
     */
    fssize: number;
    zfs: {
        /**
         * ZFS root path
         */
        zfsroot: string
    }; /*  */
    lvm: {
        /**
         * LVM Volume Group name
         */
        vg: string;
        /**
         * LVM Logical Volume name
         */
        lv: string;
        /**
         * LVM thin pool to use, if any
         */
        thinpool: string;
    };
    /**
     * Directory path
     */
    dir: string;
    rbd: {
        /**
         * RBD image name
         */
        rbdname: string;
        /**
         * Ceph pool name
         */
        rbdpool: string;
    }
}

export type Container = {
    get name(): string;
    get state(): "STOPPED" | "STARTING" | "RUNNING" | "ABORTING" | "STOPPING";
    get defined(): boolean;

    new(name: string, configPath?: string): Container;

    start(useinit: number, argv: string[]): Promise<void>;
    stop(): boolean;
    create(template: string, bdevtype: string, bdev_specs: Partial<bdev_specs>, flags: number, argv: string[]): Promise<void>;
    getConfigItem(key: string): string;
    setConfigItem(key: string, value: string): boolean;
    clearConfigItem(key: string): boolean;
    clearConfig(): void;

    attach(clear_env: boolean, namespace: number, personality: number, uid: number, guid: number, groups: number[], stdio: [number, number, number], cwd: string, env: string[], keep_env: string[], flags: number): Promise<number>;
    exec(clear_env: boolean, namespace: number, personality: number, uid: number, guid: number, groups: number[], stdio: [number, number, number], cwd: string, env: string[], keep_env: string[], flags: number, argv: string[]): Promise<number>;

    daemonize(v: boolean): boolean;
}
//endregion

//region type bindings
export const Container: Container = <Container>bindings.Container;
export const LXC = {
    get version(): string {
        return bindings.GetVersion();
    },
    getGlobalConfigItem(key: string): string {
        return bindings.getGlobalConfigItem(key);
    },
    get listAllContainerNames(): string[] {
        return bindings.ListAllContainers();
    },
    get listAllDefinedContainerNames(): string[] {
        return bindings.ListAllDefinedContainers();
    },
    get listAllActiveContainerNames(): string[] {
        return bindings.ListAllActiveContainers();
    }
}
