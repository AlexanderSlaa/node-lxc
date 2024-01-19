import {CommonOptions, OptionMapper} from "./index";
import {execFile, ExecOptions} from "child_process";

export type DeviceOptions = {
    /**
     * NAME of the container
     */
    name: string,
    /**
     * Load configuration file FILE
     */
    rcfile?: string,
    operation: "add" | "del",
    dev: string
} & CommonOptions

export const DeviceOptionMapper ={
    "operation": (key:string, value: string) => {
        return [value]
    }
}

export const device = (options: DeviceOptions, execOptions: ExecOptions) => execFile("lxc-device", OptionMapper(DeviceOptionMapper, options), execOptions);