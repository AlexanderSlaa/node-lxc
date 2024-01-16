import {CommonOptions} from "./index";

export type AutostartOptions = {
    kill?: true
} | {
    list?: true,
} | {
    reboot?: true,
} | {
    shutdown?: true
} & {
    all?: true,
    "ignore-auto"?: true,
    groups?: string[],
    timeout?: number
} & CommonOptions

export const AutoStartOptionMapper = {
    $: (key: string, value: any) => [`--${key}`, value],
    groups: (key: string,value: string[]) => [`--${key}`, value.join(',')]
}