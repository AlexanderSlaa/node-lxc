import {CommonOptions, OptionMapper} from "./index";
import {execFile, ExecOptions} from "child_process";

export type WaitOptions = {
    /**
     *  NAME of the container
     */
    name: string,
    /**
     * ORed states to wait for STOPPED, STARTING, RUNNING, STOPPING, ABORTING, FREEZING, FROZEN, THAWED
     */
    state: Uppercase< "STOPPED"|"STARTING"|"RUNNING"|"STOPPING"|"ABORTING"|"FREEZING"|"FROZEN"|"THAWED">[],
    /**
     * Seconds to wait for state changes
     */
    timeout?: number,
    /**
     * Load configuration file FILE
     */
    rcfile?: string
} & CommonOptions

export const WaitOptionMapper = {
    state: (key: string, value: string[]) =>[`--${key}=${value.join(", ")}`],
}

export const wait = (options: WaitOptions, execOptions: ExecOptions) => execFile("lxc-wait", OptionMapper(WaitOptionMapper, options), execOptions);