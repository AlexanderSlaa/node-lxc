import {CommonOptions, OptionMapper} from "./index";
import {execFile, ExecOptions} from "child_process";

export type MonitorOptions = {
    /**
     * NAME of the container
     * NAME may be a regular expression
     */
    name: RegExp | string,
    quit?: true
} & CommonOptions

export const MonitorOptionMapper = {
    name: (key: string, value: RegExp | string) => {
        return [`${key}=${value.toString()}`];
    }
}

export const monitor = (options: MonitorOptions, execOptions: ExecOptions) => execFile("lxc-monitor", OptionMapper(MonitorOptionMapper, options), execOptions);

