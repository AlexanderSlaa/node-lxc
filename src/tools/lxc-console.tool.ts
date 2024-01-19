import {CommonOptions, DefaultOptionMap, OptionMapper} from "./index";
import {execFile, ExecOptions} from "child_process";

export type ConsoleOptions = {
    /**
     * NAME of the container
     */
    name: string,
    /**
     * console tty number
     */
    tty?: number,
    /**
     * prefix for escape command
     */
    escape?: string,
    /**
     * Load configuration file FILE
     */
    rcfile?: string
} & CommonOptions

export const console = (options: ConsoleOptions, execOptions: ExecOptions) => execFile("lxc-console", OptionMapper(DefaultOptionMap, options), execOptions);