let bindings
if (process.env["MODE"] === "DEV") {
    console.warn("!!!!RUNNING IN DEV MODE!!!!");
    bindings = require("./../build/Debug/node-lxc.node");
} else {
    bindings = require("./../build/Release/node-lxc.node");
}

import {bdev_specs} from "./types/bdev_specs";

export * from "./constants"

//region types


export type ContainerState =
    "STOPPED"
    | "STARTING"
    | "RUNNING"
    | "STOPPING"
    | "ABORTING"
    | "FREEZING"
    | "FROZEN"
    | "THAWED";


export type Container = {
    get name(): string;

    get state(): ContainerState;

    get defined(): boolean;

    new(name: string, configPath?: string): Container;

    create(template: string, bdevtype: string, bdev_specs: Partial<bdev_specs>, flags: number, argv: string[]): Promise<void>;

    destroy(force: boolean): Promise<void>;

    start(useinit: number, argv: string[]): Promise<void>;

    stop(kill: true): boolean;

    getConfigItem(key: string): string;

    setConfigItem(key: string, value: string): boolean;

    clearConfigItem(key: string): boolean;

    clearConfig(): void;

    wait(state: ContainerState, timeout: number): Promise<void>;

    attach(clear_env: boolean, namespace: number, personality: number, uid: number, guid: number, groups: number[], stdio: [number, number, number], cwd: string, env: string[], keep_env: string[], flags: number): Promise<number>;

    exec(clear_env: boolean, namespace: number, personality: number, uid: number, guid: number, groups: number[], stdio: [number, number, number], cwd: string, env: string[], keep_env: string[], flags: number, argv: string[]): Promise<number>;

    console(ttynum: number, stdio: [number, number, number], escape: number): Promise<void>;

    daemonize(v: boolean): boolean;
}

//endregion

export type LXC = {
    GetVersion(): string,
    GetGlobalConfigItem(key: string): string
    ListAllContainer(lxcpath?: string): string[],
    ListAllDefinedContainer(lxcpath?: string): string[],
    ListAllActiveContainers(lxcpath?: string): string[],
    Container: Container
}

export const LXC: LXC = bindings;

export const Container = LXC.Container;