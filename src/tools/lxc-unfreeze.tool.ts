import {execFile, ExecOptions} from "child_process";
import {CommonOptions, DefaultOptionMap, OptionMapper} from "./index";

export type UnfreezeOptions = {
    /**
     * NAME of the container
     */
    name: string,
    /**
     * Load configuration file FILE
     */
    rcfile?: string
} & CommonOptions

export const unfreeze = (options: UnfreezeOptions, execOptions: ExecOptions) => execFile("lxc-unfreeze", OptionMapper(DefaultOptionMap, options), execOptions);
