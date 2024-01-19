import {CommonOptions, OptionMapper} from "./index";
import {execFile, execFileSync, ExecOptions, ExecSyncOptions} from "child_process";

export type AttachOptions = {
    /**
     * name of the container
     */
    name: string,
    /**
     * Use elevated privileges instead of those of the container.
     * If you don't specify privileges to be elevated as OR'd list: CAP, CGROUP and LSM (capabilities, cgroup and restrictions, respectively) then all of them will be elevated.
     * @WARNING: This may leak privileges into the container.
     * Use with care.
     */
    "elevated-privileges"?: string[],
    /**
     * Use ARCH for program instead of container's own architecture.
     */
    arch?: string,
    /**
     * Don't attach to all the namespaces of the container but just to the following OR'd list of flags MOUNT, PID, UTSNAME, IPC, USER or NETWORK.
     * WARNING: Using -s implies -e with all privileges elevated, it may therefore leak privileges into the container.
     * Use with care.
     */
    namespaces?: string[],
    /**
     * Remount /sys and /proc if not attaching to the mount namespace
     * when using -s in order to properly reflect the correct namespace context.
     * See the lxc-attach(1) manual page for details.
     */
    "remount-sys-proc"?: true,
    /**
     * Clear all environment variables before attaching.
     * The attached shell/program will start with only container=lxc set.
     */
    "clear-env"?: true,
    /**
     * Keep all current environment variables.
     * This is the current default behaviour, but is likely to change in the future.
     */
    "keep-env"?: true,
    /**
     * Log pty output to FILE
     */
    "pty-log"?: string,
    /**
     * Set an additional variable that is seen by the attached program in the container.
     * May be specified multiple times.
     */
    "set-var"?: Record<string, string>,
    /**
     * Keep an additional environment variable.
     * Only applicable if --clear-env is specified.
     * May be used multiple times.
     */
    "keep-var"?: string[]
    /**
     * Load configuration file FILE
     */
    rcfile?: string,
    /**
     * Execute COMMAND with UID inside the container
     */
    uid?: number,
    /**
     * Execute COMMAND with GID inside the container
     */
    gid?: number,
    /**
     * SELinux Context to transition into
     */
    context?: string

} & CommonOptions

export const AttachOptionMapper = {
    "set-var": (key: string, value: Record<Uppercase<string>, string>) => Object.entries(value).map(([key, value]) => `${key}=${value}`).map((_var) => [`--${key}`, _var]).flat()
}

export const attach = (options: AttachOptions, execOptions: ExecOptions) => execFile("lxc-attach", OptionMapper(AttachOptionMapper, options), execOptions);
