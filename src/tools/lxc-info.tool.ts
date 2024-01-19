import {CommonOptions, DefaultOptionMap, OptionMapper} from "./index";
import {exec, execFile, execFileSync, ExecFileSyncOptionsWithBufferEncoding, ExecOptions} from "child_process";

export type InfoOptions = {
    /**
     * NAME of the container
     */
    name: string,
    /**
     * show configuration variable KEY from running container
     */
    config?: string,
    /**
     * shows the IP addresses
     */
    ips?: true,
    /**
     * shows the process id of the init container
     */
    pid?: true,
    /**
     * shows usage stats
     */
    stats?: true,
    /**
     *  shows stats as raw numbers, not humanized
     */
    "no-humanize"?: true,
    /**
     * shows the state of the container
     */
    state?: boolean,
    /**
     * Load configuration file FILE
     */
    rcfile?: string
} & CommonOptions

export const info = (options: InfoOptions, execOptions: ExecOptions) => execFile("lxc-info", OptionMapper(DefaultOptionMap,options), execOptions)
export const infoSync = (options: InfoOptions, execOptions: ExecFileSyncOptionsWithBufferEncoding) => execFileSync("lxc-info", OptionMapper(DefaultOptionMap,options), execOptions).toString().trim();