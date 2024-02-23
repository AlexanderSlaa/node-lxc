import {lxc_clone_options} from "./types/lxc_clone_options";

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
    set name(newName: string);

    get state(): ContainerState;

    get defined(): boolean;

    get running(): boolean;

    get initPID(): number;

    get configFileName(): string;

    get daemonize(): boolean;
    set daemonize(value: boolean);

    new(name: string, configPath?: string): Container;

    create(template: string, bdevtype: string, bdev_specs: Partial<bdev_specs>, flags: number, argv: string[]): Promise<void>;

    destroy(options?: { force?: true, include_snapshots?: true }): Promise<void>;

    start(useinit: number, argv: string[]): Promise<void>;

    stop(kill: true): boolean;

    /**
     *
     * @param timeout Seconds to wait before returning false.
     * (-1 to wait forever, 0 to avoid waiting).
     */
    reboot(timeout: number): Promise<boolean>; //TODO Implement reboot send

    /**
     * Request the container shutdown by sending it `SIGPWR`.
     * @param timeout Seconds to wait before returning false.
     * (-1 to wait forever, 0 to avoid waiting).
     */
    shutdown(timeout: number): Promise<boolean>;

    getConfigItem(key: string): string;

    setConfigItem(key: string, value: string): void;

    clearConfigItem(key: string): void;

    clearConfig(): void;

    /**
     * Save configuration to a file.
     * @param alt_file Full path to file to save configuration in.
     */
    save(alt_file: string): Promise<void>;

    clone(options: lxc_clone_options): Promise<Container>;
    consoleGetFds(ttynum: number): Promise<[number, number]>

    getKeys(prefix: string): string[]
    getInterfaces(): Promise<string[]>
    getIPs(iface: string, family: "inet"): Promise<string[]>
    getIPs(iface: string, family: "inet6", scope: number): Promise<string[]>
    getCGroupItem(subsys: string): string | undefined;
    setCGroupItem(subsys: string, value: string): void;

    wait(state: ContainerState, timeout: number): Promise<void>;

    attach(clear_env: boolean, namespace: number, personality: number, uid: number, guid: number, groups: number[], stdio: [number, number, number], cwd: string, env: string[], keep_env: string[], flags: number): Promise<number>;

    exec(clear_env: boolean, namespace: number, personality: number, uid: number, guid: number, groups: number[], stdio: [number, number, number], cwd: string, env: string[], keep_env: string[], flags: number, argv: string[]): Promise<number>;

    console(ttynum: number, stdio: [number, number, number], escape: number): Promise<void>;

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