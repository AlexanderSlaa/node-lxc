import {execFileSync} from "child_process";
import {configSync} from "./src/tools/lxc-config.tool";
import {lsSync} from "./src/tools/lxc-ls.tool";

export * from "./src/types/Container";

export const MODE = "CLI";


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
    fstype: string;	 /*!< Filesystem type */
    fssize: number;/*!< Filesystem size in bytes */
    zfs: { zfsroot: string }; /*!< ZFS root path */
    lvm: {
        vg: string;		/*!< LVM Volume Group name */
        lv: string;		/*!< LVM Logical Volume name */
        thinpool: string; /*!< LVM thin pool to use, if any */
    };
    dir: string; /*!< Directory path */
    rbd: {
        rbdname: string; /*!< RBD image name */
        rbdpool: string; /*!< Ceph pool name */
    }
}


//endregion

//region type bindings
export const LXC = {
    get version(): string {
        return execFileSync("lxc-ls",["--version"]).toString().trim();
    },
    getGlobalConfigItem(key: string): string {
        return configSync({item: key}, {});
    },
    get listAllContainerNames(): string[] {
        return lsSync({}, {});
    },
    get listAllDefinedContainerNames(): string[] {
        return lsSync({defined: true}, {});
    },
    get listAllActiveContainerNames(): string[] {
        return lsSync({active: true}, {});
    }
}
