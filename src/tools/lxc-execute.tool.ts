import {CommonOptions, DefaultOptionMap, OptionMapper} from "./index";
import {execFile, ExecOptions} from "child_process";

export type ExecuteOptions = {
    /**
     * NAME of the container
     */
    name: string,
    /**
     * Daemonize the container
     */
    daemon?: true,
    /**
     * Load configuration file FILE
     */
    rcfile?: string,
    /**
     * Assign VAL to configuration variable KEY
     */
    define?: `${string}=${string}`,
    /**
     * Execute COMMAND with UID inside the container
     */
    uid?: number,
    /**
     *  Execute COMMAND with GID inside the container
     */
    gid?: number
} & CommonOptions

export const execute = (options: ExecuteOptions, execOptions: ExecOptions) => execFile("lxc-execute", OptionMapper(DefaultOptionMap, options), execOptions);