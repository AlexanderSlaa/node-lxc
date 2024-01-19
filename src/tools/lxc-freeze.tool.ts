import {execFile, ExecOptions} from "child_process";
import {CommonOptions, DefaultOptionMap, OptionMapper} from "./index";

export type FreezeOptions = {
    /**
     * NAME of the container
     */
    name: string,
    /**
     * Load configuration file FILE
     */
    rcfile?: string
} & CommonOptions

export const freeze = (options: FreezeOptions, execOptions: ExecOptions) => execFile("lxc-freeze", OptionMapper(DefaultOptionMap, options), execOptions);