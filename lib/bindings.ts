import type {
    bdev_specs, lxc_attach_options, lxc_clone_options,
    lxc_console_log, LXC_CREATE, LXC_MIGRATE, lxc_mount, lxc_snapshot, migrate_opts
} from "./types";

export * from "./types"

const bindings = process.env["NODE_ENV"] === "development" ? require("./../build/Debug/node-lxc.node") : require("./../build/Release/node-lxc.node");

if (process.env["NODE_ENV"] === "development") {
    console.warn("!!!RUNNING IN DEVELOPMENT MODE!!!");
}


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
    /**
     * Name of container
     * @returns {string} current name of container
     */
    get name(): string;
    /**
     * Rename a container
     * @param newName {string} New name to be used for the container.
     * @link [https://linuxcontainers.org/apidoc](https://linuxcontainers.org/lxc/apidoc/structlxc__container.html#a369207c40b4946a04026861bfef3d86c)
     */
    set name(newName: string);
    /**
     * Determine if /var/lib/lxc/$name/config exists.
     * @link [https://linuxcontainers.org/apidoc](https://linuxcontainers.org/lxc/apidoc/structlxc__container.html#a84fd83757b47401b95ff01efae76d896)
     */
    get defined(): boolean;
    /**
     * Determine state of container.
     * @returns `true` if defined `false` if not
     * @link [https://linuxcontainers.org/apidoc](https://linuxcontainers.org/lxc/apidoc/structlxc__container.html#aa071116fd2f39c4cbe62d171566d14b9)
     */
    get state(): ContainerState;
    /**
     * Determine if container is running.
     * @returns `true` if running `false` if not
     * @link [https://linuxcontainers.org/apidoc](https://linuxcontainers.org/lxc/apidoc/structlxc__container.html#ab810a7cd940111dbb480eb0440f71abe)
     */
    get running(): boolean;
    /**
     * Determine process ID of the containers init process.
     * @returns pid of init process as seen from outside the container.
     * @link [https://linuxcontainers.org/apidoc](https://linuxcontainers.org/lxc/apidoc/structlxc__container.html#a8ce63dc530f57a3a198ed2d0f0c229a9)
     */
    get initPID(): number;
    /**
     * Current config file name.
     * @returns {string} config file name.
     * @link [https://linuxcontainers.org/apidoc]()
     */
    get configFileName(): string;

    /**
     * Whether container wishes to be daemonized
     * @returns {boolean}
     * @link [https://linuxcontainers.org/apidoc](https://linuxcontainers.org/lxc/apidoc/structlxc__container.html#aad6d903f5f22937f0662814ee4c1ac05)
     */
    get daemonize(): boolean;
    /**
     * Whether container wishes to be daemonized
     * @param value {boolean}
     * @link [https://linuxcontainers.org/apidoc](https://linuxcontainers.org/lxc/apidoc/structlxc__container.html#aad6d903f5f22937f0662814ee4c1ac05)
     */
    set daemonize(value: boolean);

    /**
     * Determine full path to the containers configuration file.
     * Each container can have a custom configuration path.
     * However, by default it will be set to either the LXCPATH configure variable, or the lxcpath value in the LXC_GLOBAL_CONF configuration file (i.e. /etc/lxc/lxc.conf).
     * @returns full path to configuration file
     * @link [https://linuxcontainers.org/apidoc](https://linuxcontainers.org/lxc/apidoc/structlxc__container.html#a7244de1a9590f9650bf75b23f1fce0ee)
     */
    get configPath(): string;
    /**
     * Set the full path to the containers configuration file.
     * @param path {string} Full path to configuration file.
     * @link [https://linuxcontainers.org/apidoc](https://linuxcontainers.org/lxc/apidoc/structlxc__container.html#ad140523960327ab5537629ee8dc62dd3)
     */
    set configPath(path: string);

    new(name: string, configPath?: string, alt_file?: string): Container;

    /**
     * Freeze running container.
     * @returns {Promise<void>} if there is any error the promise is rejected
     * @link [https://linuxcontainers.org/apidoc](https://linuxcontainers.org/lxc/apidoc/structlxc__container.html#ab83d343ed927520ee693c3c0fc3514ca)
     */
    freeze(): Promise<void>;
    /**
     * Thaw a frozen container.
     * @returns {Promise<void>} if there is any error the promise is rejected
     * @link [https://linuxcontainers.org/apidoc](https://linuxcontainers.org/lxc/apidoc/structlxc__container.html#ac1710a6788be63a2429918ec3724641e)
     */
    unfreeze(): Promise<void>;
    /**
     * Load the specified configuration for the container.
     * @param alt_file {string} Full path to alternate configuration file
     * @returns {Promise<void>} if there is any error the promise is rejected
     */
    loadConfig(alt_file: string): Promise<void>;
    /**
     * Start the container.
     * @link [https://linuxcontainers.org/apidoc](https://linuxcontainers.org/lxc/apidoc/structlxc__container.html#a599d630c220ee5ef2a9c6b79c7eb3b23)
     * @param useinit {boolean} Use lxcinit rather than /sbin/init.
     * @param argv {string[]} Array of arguments to pass to init.
     */
    start(useinit?: number, argv?: string[]): Promise<void>;
    /**
     * Stop the container.
     * @link [https://linuxcontainers.org/apidoc](https://linuxcontainers.org/lxc/apidoc/structlxc__container.html#a661d604dd5d29b5d6dd5d141d63c8539)
     * @returns {Promise<void>} if there is any error the promise is rejected
     */
    stop(): Promise<void>;
    /**
     * Change whether the container wishes all file descriptors to be closed on startup.
     * @link [https://linuxcontainers.org/apidoc](//TODO)
     * @param state {boolean} Value for the close_all_fds.
     * @returns {Promise<void>} if there is any error the promise is rejected
     */
    wantCloseAllFds(state: boolean): Promise<void>; //Check if state is used in c bindings
    /**
     * Wait for a container to be in a specified state
     * @link [https://linuxcontainers.org/apidoc](//TODO)
     * @param state {ContainerState} State to wait for.
     * @param timeout {number} default: -1 Timeout in seconds.
     * @returns {Promise<void>} When resolved the container is the specified state, when rejected the container wait timed-out or an error occurred
     */
    wait(state: ContainerState, timeout?: number): Promise<void>;
    /**
     * Set a key/value configuration option.
     * @param key {string} config key
     * @param value {string} config value
     * @link [https://linuxcontainers.org/apidoc](https://linuxcontainers.org/lxc/apidoc/structlxc__container.html#a340914109541781444138be66b4e88eb)
     * @example
     * c.setConfigItem('lxc.log.file', '/tmp/logs/ct2.log');
     */
    setConfigItem(key: string, value: string): void;
    /**
     * Delete the container.
     * @link [https://linuxcontainers.org/apidoc](https://linuxcontainers.org/lxc/apidoc/structlxc__container.html#a284790c42e39e597eae61ae2d2995173)
     * @param options
     * @returns {Promise<void>} if there is any error the promise is rejected
     * @note **Container must be stopped and have no dependent snapshots.**
     */
    destroy(options?: { include_snapshots?: boolean, force?: boolean }): Promise<void>
    /**
     * Save configuration to a file.
     * @link [https://linuxcontainers.org/apidoc](https://linuxcontainers.org/lxc/apidoc/structlxc__container.html#abea85657b8f816e7d2d5238c0d731a67)
     * @param alt_file Full path to file to save configuration in.
     * @returns {Promise<void>} if there is any error the promise is rejected
     */
    save(alt_file: string): Promise<void>;
    /**
     * Create a container.
     * @link [https://linuxcontainers.org/apidoc](https://linuxcontainers.org/lxc/apidoc/structlxc__container.html#ad2a402be12791e43dd016bba24e53b40)
     * @returns {Promise<void>} if there is any error the promise is rejected
     * @param options
     */
    create(options: {
        /**
         * Template to execute to instantiate the root filesystem and adjust the configuration.
         */
        template?: string,
        /**
         *    Arguments to pass to the template
         */
        argv?: string[]
        /**
         *    Backing store type to use
         *    @default dir
         */
        bdevtype?: string,
        /**
         * Additional parameters for the backing store (for example LVM volume group to use).
         */
        bdev_specs?: Partial<bdev_specs>,
        /**
         * Additional parameters for the backing store (for example LVM volume group to use).
         */
        flags?: LXC_CREATE
    }): Promise<void>;
    /**
     * Request the container reboot by sending it SIGINT.
     * @param timeout Seconds to wait before returning false.
     * (-1 to wait forever, 0 to avoid waiting).
     * default: -1 (forever)
     * @link [https://linuxcontainers.org/apidoc](https://linuxcontainers.org/lxc/apidoc/structlxc__container.html#a5c6f030b9e8797cf9a4586b1fab6c3eb)
     */
    reboot(timeout?: number): Promise<boolean>; //TODO Implement reboot send
    /**
     * Request the container shutdown by sending it `SIGPWR`.
     * @param timeout Seconds to wait before rejecting (-1 to wait forever, 0 to avoid waiting).
     * default: -1 (forever)
     * @link [https://linuxcontainers.org/apidoc](https://linuxcontainers.org/lxc/apidoc/structlxc__container.html#a9cfba9429c159612a8d6b768f6e27ade)
     */
    shutdown(timeout?: number): Promise<boolean>;
    /**
     * Completely clear the containers in-memory configuration.
     * @link [https://linuxcontainers.org/apidoc](https://linuxcontainers.org/lxc/apidoc/structlxc__container.html#a2d40919280dbedd9d29417d5ca117e86)
     */
    clearConfig(): void;
    /**
     * Clear a configuration item.
     * @link [https://linuxcontainers.org/apidoc](https://linuxcontainers.org/lxc/apidoc/structlxc__container.html#a4521fb10eee10cf6ca9b33d3ef1f996a)
     * @param key {string} Name of option to clear.
     * @see analog of {setConfigItem}
     */
    clearConfigItem(key: string): void;
    /**
     * Retrieve the value of a config item.
     * @link [https://linuxcontainers.org/apidoc](https://linuxcontainers.org/lxc/apidoc/structlxc__container.html#a7411db2a0e7a8471c41a4a58d8e83b33)
     * @param key {string} Name of option to get.
     */
    getConfigItem(key: string): string;
    /**
     * Retrieve the value of a config item from running container.
     * @link [https://linuxcontainers.org/apidoc](https://linuxcontainers.org/lxc/apidoc/structlxc__container.html#aeb0f5f1589ba5dc14d5d1e1c0cb87f96)
     * @param key
     */
    getRunningConfigItem(key: string): string;
    /**
     * Retrieve a list of config item keys given a key prefix.
     * @link [https://linuxcontainers.org/apidoc](https://linuxcontainers.org/lxc/apidoc/structlxc__container.html#a885bbd0d02c3771bcee7b02a2c398be9)
     * @param prefix
     */
    getKeys(prefix: string): string[]
    /**
     * Obtain a list of network interfaces.
     * @link [https://linuxcontainers.org/apidoc](https://linuxcontainers.org/lxc/apidoc/structlxc__container.html#a50d09300fa69182156b58eb9ef288dbd)
     */
    getInterfaces(): Promise<string[]>
    /**
     * Determine the list of container IPv4 addresses.
     * @link [https://linuxcontainers.org/apidoc](https://linuxcontainers.org/lxc/apidoc/structlxc__container.html#a70b47b5719463e4c61ed097b285d9371)
     * @param iface {string} interface name
     * @param family {string} address family inet
     */
    getIPs(iface: string, family: "inet"): Promise<string[]>
    /**
     * Determine the list of container IPv6 addresses.
     * @link [https://linuxcontainers.org/apidoc](https://linuxcontainers.org/lxc/apidoc/structlxc__container.html#a70b47b5719463e4c61ed097b285d9371)
     * @param iface {string} interface name
     * @param family {string} address family
     * @param scope {string} IPv6 scope id
     */
    getIPs(iface: string, family: "inet6", scope: number): Promise<string[]>
    /**
     * Retrieve the specified cgroup subsystem value for the container.
     * @link [https://linuxcontainers.org/apidoc](https://linuxcontainers.org/lxc/apidoc/structlxc__container.html#a6202bf97cd77e208eed1845d5a911cbc)
     * @param subsys {string} cgroup subsystem to retrieve.
     */
    getCGroupItem(subsys: string): string | undefined;
    /**
     * Set the specified cgroup subsystem value for the container.
     * @link [https://linuxcontainers.org/apidoc](//TODO)
     * @param subsys {string} cgroup subsystem to consider.
     * @param value {string} Value to set for `subsys`.
     */
    setCGroupItem(subsys: string, value: string): void;
    /**
     * Copy a stopped container.
     * @link [https://linuxcontainers.org/apidoc](//TODO)
     * @param options {lxc_clone_options}
     * @note
     * If devtype was not specified, and flags contains LXC_CLONE_SNAPSHOT then use the native bdevtype if possible, else use an overlayfs.
     */
    clone(options: lxc_clone_options): Promise<Container>;
    /**
     * Allocate a console tty for the container.
     * @link [https://linuxcontainers.org/apidoc](https://linuxcontainers.org/lxc/apidoc/structlxc__container.html#a64b4ce75b807b21e42d4ccd024652973)
     * @param ttynum
     * @returns {Promise<[number, number]>} [ttynum, ptxfd]
     * @note
     * On successful return, ttynum will contain the tty number that was allocated.
     * The returned file descriptor is used to keep the tty allocated.
     * The caller should call close(2) on the returned file descriptor when no longer required so that it may be allocated by another caller.
     */
    consoleGetFds(ttynum?: number): Promise<[number, number]>
    /**
     * Allocate and run a console tty.
     * @link [https://linuxcontainers.org/apidoc](//TODO)
     * @param ttynum {number} Terminal number to attempt to allocate, -1 to allocate the first available tty or 0 to allocate the console.
     * @param stdio {[number,number,number]} [stdinfd, stdoutfd, stderrfd]
     * @param escape {number} The escape character (1 == 'a', 2 == 'b', ...).
     *
     * @note This function will not return until the console has been exited by the user.
     */
    console(ttynum: number, stdio: [number, number, number], escape: number): Promise<void>;
    /**
     * @link [https://linuxcontainers.org/apidoc](//TODO)
     * @param options {lxc_attach_options}
     */
    // attach(clear_env: boolean, namespace: number, personality: number, uid: number, guid: number, groups: number[], stdio: [number, number, number], cwd: string, env: string[], keep_env: string[], flags: number): Promise<number>;
    attach(options?: Partial<lxc_attach_options>): Promise<number>;
    /**
     * @link [https://linuxcontainers.org/apidoc](//TODO)
     * @param commentfile
     */
    snapshot(commentfile: string): Promise<number>;
    /**
     * @link [https://linuxcontainers.org/apidoc](//TODO)
     */
    snapshotList(): Promise<lxc_snapshot[]>
    /**
     * @link [https://linuxcontainers.org/apidoc](//TODO)
     * @param snapname
     * @param newname
     */
    snapshotRestore(snapname: string, newname?: string): Promise<void>
    /**
     * @link [https://linuxcontainers.org/apidoc](//TODO)
     * @param snapname
     */
    snapshotDestroy(snapname: string): Promise<void>
    /**
     * @link [https://linuxcontainers.org/apidoc](//TODO)
     * @param all
     */
    snapshotDestroy(all: true): Promise<void>
    /**
     * @link [https://linuxcontainers.org/apidoc](//TODO)
     * @param src_path
     * @param dest_path
     */
    addDeviceNode(src_path: string, dest_path?: string): Promise<void>
    /**
     * @link [https://linuxcontainers.org/apidoc](//TODO)
     * @param src_path
     * @param dest_path
     */
    removeDeviceNode(src_path: string, dest_path?: string): Promise<void>
    /**
     * @link [https://linuxcontainers.org/apidoc](//TODO)
     * @param dev
     * @param dst_dev
     */
    attachInterface(dev: string, dst_dev?: string): Promise<void>
    /**
     * @link [https://linuxcontainers.org/apidoc](//TODO)
     * @param dev
     * @param dst_dev
     */
    detachInterface(dev: string, dst_dev?: string): Promise<void>
    /**
     * @link [https://linuxcontainers.org/apidoc](//TODO)
     * @param directory
     * @param stop
     * @param verbose
     */
    checkpoint(directory: string, stop?: boolean, verbose?: boolean): Promise<void>
    /**
     * @link [https://linuxcontainers.org/apidoc](//TODO)
     * @param directory
     * @param verbose
     */
    restore(directory: string, verbose?: boolean): Promise<void>
    /**
     * @link [https://linuxcontainers.org/apidoc](//TODO)
     * @param cmd
     * @param options
     */
    migrate(cmd: LXC_MIGRATE, options?: Partial<migrate_opts>): Promise<void>
    /**
     * @link [https://linuxcontainers.org/apidoc](//TODO)
     * @param options
     */
    consoleLog(options?: Partial<lxc_console_log>): Promise<string>
    /**
     * @link [https://linuxcontainers.org/apidoc](//TODO)
     * @param source
     * @param target
     * @param filesystemtype
     * @param mountflags
     * @param mnt
     */
    mount(source: string, target: string, filesystemtype: string | undefined, mountflags: bigint | number, mnt: lxc_mount): Promise<void>
    /**
     * @link [https://linuxcontainers.org/apidoc](//TODO)
     * @param source
     * @param mountflags
     * @param mnt
     */
    umount(source: string, mountflags: bigint | number, mnt: lxc_mount): Promise<void>
    /**
     * @link [https://linuxcontainers.org/apidoc](//TODO)
     */
    seccompNotifyFd(): Promise<number>
    /**
     * @link [https://linuxcontainers.org/apidoc](//TODO)
     */
    seccompNotifyFdActive(): Promise<number>
    /**
     * @link [https://linuxcontainers.org/apidoc](//TODO)
     */
    initPIDFd(): Promise<number>
    /**
     * @link [https://linuxcontainers.org/apidoc](//TODO)
     */
    devptsFd(): Promise<number>
    /**
     * @link [https://linuxcontainers.org/apidoc](//TODO)
     * @param clear_env
     * @param namespace
     * @param personality
     * @param uid
     * @param guid
     * @param groups
     * @param stdio
     * @param cwd
     * @param env
     * @param keep_env
     * @param flags
     * @param argv
     */
    exec(clear_env: boolean, namespace: number, personality: number, uid: number, guid: number, groups: number[], stdio: [number, number, number], cwd: string, env: string[], keep_env: string[], flags: number, argv: string[]): Promise<number>;
}


//endregion

export type LXC = {
    /**
     * @link [https://linuxcontainers.org/apidoc](//TODO)
     * @constructor
     */
    GetVersion(): string,
    /**
     * @link [https://linuxcontainers.org/apidoc](//TODO)
     * @param key
     * @constructor
     */
    GetGlobalConfigItem(key: string): string
    /**
     * @link [https://linuxcontainers.org/apidoc](//TODO)
     * @param lxcpath
     * @constructor
     */
    ListAllContainers(lxcpath?: string): string[],
    /**
     * @link [https://linuxcontainers.org/apidoc](//TODO)
     * @param lxcpath
     * @constructor
     */
    ListAllDefinedContainer(lxcpath?: string): string[],
    /**
     * @link [https://linuxcontainers.org/apidoc](//TODO)
     * @param lxcpath
     * @constructor
     */
    ListAllActiveContainers(lxcpath?: string): string[],
    /**
     * @link [https://linuxcontainers.org/apidoc](//TODO)
     */
    Container: Container
}

export const LXC: LXC = bindings;

export const Container = LXC.Container;