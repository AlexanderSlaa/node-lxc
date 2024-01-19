import {execFile, ExecOptions} from "child_process";
import {type CommonOptions, OptionMapper} from "./index";

export type CGroupOptions = {
    name: string,
    rcfile?: string,
    "state-object"?: string,
    "value"?: string
} & CommonOptions

export const CGroupOptionMapper = {
    "state-object": (key: string, value: any) => {
        return [value];
    }
}

export const cgroup = (options: CGroupOptions, execOptions: ExecOptions) => execFile("lxc-cgroup", OptionMapper(CGroupOptionMapper, options), execOptions);