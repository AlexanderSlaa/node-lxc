import {execFile, execFileSync, ExecOptions} from "child_process";
import {CommonOptions, OptionMapper} from "./index";

export type ConfigOptions = ({
    /**
     * list all available configuration items
     */
    "l"?: true
} | {
    /**
     * print configuration item
     */
    item?: string
}) & CommonOptions

export const ConfigOptionMapper = {
    "l": (key: string, value: any) => {
        return [`-${key}`];
    },
    item: (key: string, value: any) => {
        return [value];
    }
}

export const config = (options: ConfigOptions, execOptions: ExecOptions) => execFile("lxc-config", OptionMapper(ConfigOptionMapper, options), execOptions);
export const configSync = (options: ConfigOptions, execOptions: ExecOptions) => execFileSync("lxc-config", OptionMapper(ConfigOptionMapper, options), execOptions).toString();