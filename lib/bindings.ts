import {lxc_clone_options} from "./types/lxc_clone_options";

let bindings
if (process.env["MODE"] === "DEV") {
    console.warn("!!!!RUNNING IN DEV MODE!!!!");
    bindings = require("./../build/Debug/node-lxc.node");
} else {
    bindings = require("./../build/Release/node-lxc.node");
}

import {bdev_specs} from "./types/bdev_specs";
import {lxc_snapshot} from "./types/lxc_snapshot";
import {LXC_MIGRATE} from "./constants";
import {migrate_opts} from "./types/migrate_opts";
import {lxc_console_log} from "./types/lxc_console_log";
import {lxc_mount} from "./types/lxc_mount";

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
    get defined(): boolean;
    get state(): ContainerState;
    get running(): boolean;
    get initPID(): number;

    get configFileName(): string;

    get daemonize(): boolean;
    set daemonize(value: boolean);

    get configPath(): string;
    set configPath(value: string);

    new(name: string, configPath?: string, alt_file?: string): Container;

    freeze(): Promise<void>;
    unfreeze(): Promise<void>;
    loadConfig(alt_file: string): Promise<void>;


    start(useinit?: number, argv?: string[]): Promise<void>;
    stop(): Promise<void>;

    wantCloseAllFds(): Promise<void>;
    /**
     * Wait for a container to be in a specified state
     * @param state {ContainerState}
     * @param timeout {number} default: -1
     * @returns {Promise<void>} When resolved the container is the specified state, when rejected the container wait timed-out or an error occurred
     */
    wait(state: ContainerState, timeout?: number): Promise<void>;
    /**
     *
     * @param key {string} config key
     * @param value {string} config value
     * @example
     * c.setConfigItem('lxc.log.file', '/tmp/logs/ct2.log');
     */
    setConfigItem(key: string, value: string): void;
    destroy(options?: { include_snapshots?: boolean, force?: boolean }): Promise<void>
    /**
     * Save configuration to a file.
     * @param alt_file Full path to file to save configuration in.
     */
    save(alt_file: string): Promise<void>;

    create(template: string, bdevtype: string, bdev_specs: Partial<bdev_specs>, flags: number, argv: string[]): Promise<void>;
    /**
     *
     * @param timeout Seconds to wait before returning false.
     * (-1 to wait forever, 0 to avoid waiting).
     * default: -1 (forever)
     */
    reboot(timeout?: number): Promise<boolean>; //TODO Implement reboot send
    /**
     * Request the container shutdown by sending it `SIGPWR`.
     * @param timeout Seconds to wait before rejecting (-1 to wait forever, 0 to avoid waiting).
     * default: -1 (forever)
     */
    shutdown(timeout?: number): Promise<boolean>;
    clearConfig(): void;
    clearConfigItem(key: string): void;
    getConfigItem(key: string): string;
    getRunningConfigItem(key: string): string;
    getKeys(prefix: string): string[]
    getInterfaces(): Promise<string[]>
    getIPs(iface: string, family: "inet"): Promise<string[]>
    getIPs(iface: string, family: "inet6", scope: number): Promise<string[]>
    getCGroupItem(subsys: string): string | undefined;
    setCGroupItem(subsys: string, value: string): void;
    clone(options: lxc_clone_options): Promise<Container>;
    consoleGetFds(ttynum?: number): Promise<[number, number]>
    console(ttynum: number, stdio: [number, number, number], escape: number): Promise<void>;

    attach(clear_env: boolean, namespace: number, personality: number, uid: number, guid: number, groups: number[], stdio: [number, number, number], cwd: string, env: string[], keep_env: string[], flags: number): Promise<number>;
    snapshot(commentfile: string): Promise<number>;
    snapshotList(): Promise<lxc_snapshot[]>
    snapshotRestore(snapname: string, newname?: string): Promise<void>
    snapshotDestroy(snapname: string): Promise<void>
    snapshotDestroy(all: true): Promise<void>

    addDeviceNode(src_path: string, dest_path?: string): Promise<void>
    removeDeviceNode(src_path: string, dest_path?: string): Promise<void>

    attachInterface(dev: string, dst_dev?: string): Promise<void>
    detachInterface(dev: string, dst_dev?: string): Promise<void>
    checkpoint(directory: string, stop?: boolean, verbose?: boolean): Promise<void>
    restore(directory: string, verbose?: boolean): Promise<void>
    migrate(cmd: LXC_MIGRATE, options?: Partial<migrate_opts>): Promise<void>
    consoleLog(options?: Partial<lxc_console_log>): Promise<string>
    mount(source: string, target: string, filesystemtype: string | undefined, mountflags: bigint | number, mnt: lxc_mount): Promise<void>
    umount(source: string, mountflags: bigint | number, mnt: lxc_mount): Promise<void>

    seccompNotifyFd(): Promise<number>
    seccompNotifyFdActive(): Promise<number>
    initPIDFd(): Promise<number>
    devptsFd(): Promise<number>

    exec(clear_env: boolean, namespace: number, personality: number, uid: number, guid: number, groups: number[], stdio: [number, number, number], cwd: string, env: string[], keep_env: string[], flags: number, argv: string[]): Promise<number>;
}


//endregion

export type LXC = {
    GetVersion(): string,
    GetGlobalConfigItem(key: string): string
    ListAllContainers(lxcpath?: string): string[],
    ListAllDefinedContainer(lxcpath?: string): string[],
    ListAllActiveContainers(lxcpath?: string): string[],
    Container: Container
}

export const LXC: LXC = bindings;

export const Container = LXC.Container;