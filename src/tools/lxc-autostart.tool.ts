import {CommonOptions, OptionMapper} from "./index";
import {execFile, ExecOptions} from "child_process";

export type AutostartOptions = {
    /**
     * kill the containers instead of starting them
     */
    kill?: true
} | {
    /**
     * list all affected containers and wait delay
     */
    list?: true,
} | {
    /**
     * reboot the containers instead of starting them
     */
    reboot?: true,
} | {
    /**
     *  shutdown the containers instead of starting them
     */
    shutdown?: true
} & {
    /**
     * list all auto-started containers (ignore groups)
     */
    all?: true,
    /**
     * ignore lxc.start.auto and select all matching containers
     */
    "ignore-auto"?: true,
    /**
     * list of groups (comma separated) to select
     */
    groups?: string[],
    /**
     * wait T seconds before hard-stopping
     */
    timeout?: number
} & CommonOptions

export const AutoStartOptionMapper = {
    groups: (key: string,value: string[]) => [`--${key}`, value.join(',')]
}
export const autostart = (options: AutostartOptions, execOptions: ExecOptions) => execFile("lxc-autostart", OptionMapper(AutoStartOptionMapper, options), execOptions);