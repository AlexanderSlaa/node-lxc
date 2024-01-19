import {CommonOptions, DefaultOptionMap, OptionMapper} from "./index";
import {exec, execFile, execFileSync, ExecFileSyncOptions, ExecOptions, ExecSyncOptions} from "child_process";

export type StartOptions = {
    /**
     * NAME of the container
     */
    name: string,
    /**
     * Daemonize the container
     * @default true
     */
    daemon?: false,
    /**
     * Start with the current tty attached to /dev/console
     */
    foreground?: true,
    /**
     * Create a file with the process id
     */
    pidfile?: string,
    /**
     * Load configuration file FILE
     */
    rcfile?: string,
    /**
     * Use specified FILE for the container console
     */
    console?: string,
    /**
     * Log container console output to FILE
     */
    "console-log"?: string,
    /**
     * If any fds are inherited, close them
     * @note: --daemon implies --close-all-fds
     */
    "close-all-fds"?: true,
    /**
     * Assign VAL to configuration variable KEY
     */
    define?: string,
    /**
     * NAME Share a namespace with another container or pid
     */
    "share-net"?: true,
    /**
     * NAME Share a namespace with another container or pid
     */
    "share-ipc"?: true,
    /**
     * NAME Share a namespace with another container or pid
     */
    "share-uts"?: true,
    /**
     * NAME Share a namespace with another container or pid
     */
    "share-pid"?: true,
    argv?: string[]
} & CommonOptions

export const StartOptionsMapper = {
    "argv": (key: string, value: string[]) => {
        return value;
    }
}

export const start = (options: StartOptions, execOptions: ExecOptions) => execFile("lxc-start", OptionMapper(StartOptionsMapper, options), execOptions);
export const startSync = (options: StartOptions, execOptions: ExecFileSyncOptions) => execFileSync("lxc-start", OptionMapper(StartOptionsMapper, options), execOptions);