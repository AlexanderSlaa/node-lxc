import {CommonOptions, DefaultOptionMap, OptionMapper} from "./index";
import {execFile, execFileSync, ExecOptions, ExecSyncOptions} from "child_process";

export type DestroyOptions = {
    /**
     * NAME of the container
     */
    name: string,
    /**
     * destroy including all snapshots
     */
    snapshots?: true,
    /**
     * stop and destroy the container if it's still running
     */
    force?: true,
    /**
     * Load configuration file FILE
     */
    rcfile?: string,
} & CommonOptions

export const destroy = (options: DestroyOptions, execOptions: ExecOptions) => execFile("lxc-destroy", OptionMapper(DefaultOptionMap, options), execOptions);
export const destroySync = (options: DestroyOptions, execOptions: ExecSyncOptions) => execFileSync("lxc-destroy", OptionMapper(DefaultOptionMap, options), execOptions).toString();
