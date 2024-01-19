import {CommonOptions, OptionMapper} from "./index";
import {execFile, ExecOptions} from "child_process";

export type UnshareOptions = {
    /**
     *  ORed list of flags to unshare: MOUNT, PID, UTSNAME, IPC, USER, NETWORK
     */
    namespaces: Uppercase<string>[],
    /**
     * new id to be set if -s USER is specified
     */
    user?:  number,
    /**
     * Set the hostname in the container
     */
    hostname?: string,
    /**
     * Interface name to be moved into container (presumably with NETWORK unsharing set)
     */
    ifname?: string,
    /**
     * Daemonize (do not wait for container to exit)
     */
    daemon?: true,
    /**
     * Remount default fs inside container (/proc /dev/shm /dev/mqueue)
     */
    remount?: true,
    command: string,
} & CommonOptions

export const UnshareOptionMapper = {
    namespaces: (key: string, value: string[]) =>[`--${key}=${value.join(", ")}`],
    command: (key: string, value: string) => [value]
}

export const unshare = (options: UnshareOptions, execOptions: ExecOptions) => execFile("lxc-unshare", OptionMapper(UnshareOptionMapper, options), execOptions);
