import {execFile, execFileSync, ExecOptions} from "child_process";
import {CommonOptions, OptionMapper} from "./index";

export type LsOptions = {
    /**
     * list only active containers
     */
    active?: true,
    /**
     * list only running containers
     */
    running?: true,
    /**
     * list only frozen containers
     */
    frozen?: true,
    /**
     * list only stopped containers
     */
    stopped?: true,
    /**
     * list only defined containers
     */
    defined?: true,
    /**
     * list nested containers up to NUM (default is 5) levels of nesting
     * @default 5
     */
    nesting?: number
    /**
     * filter container names by regular expression
     */
    filter?: RegExp | string
    /**
     * comma separated list of groups a container must have to be displayed
     */
    groups?: true
} & CommonOptions

export const LsOptionMapper = {
    filter: (key: string, value: RegExp | string) => {
        return [`${key}=${value.toString()}`];
    }
}

export const ls = (options: LsOptions, execOptions: ExecOptions) => execFile("lxc-ls", OptionMapper(LsOptionMapper, {...options, 'line': true}), execOptions);
export const lsSync = (options: LsOptions, execOptions: ExecOptions) => execFileSync("lxc-ls", OptionMapper(LsOptionMapper, {...options, 'line': true}), execOptions).toString().split("\n").map(line => line.trim());
