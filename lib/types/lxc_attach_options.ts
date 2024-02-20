import { Personality } from "./Personality"


export type lxc_attach_options = ({
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
        elevated_privileges: boolean,
        /**
         * File descriptor to log output.
         */
        log_fd: number,
        /**
         * lsm label to set.
         */
        lsm_label: string
    }