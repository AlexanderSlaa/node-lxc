const bindings = require("./build/Release/node-lxc.node");

export enum LXC_LOGLEVEL {
    TRACE = "TRACE",
    DEBUG = "DEBUG",
    INFO = "INFO",
    NOTICE = "NOTICE",
    WARN = "WARN",
    ERROR = "ERROR",
    CRIT = "CRIT",
    ALERT = "ALERT",
    FATAL = "FATAL",
}

export enum LXC_CREATE {
    QUIET = (1 << 0),
    MAXFLAGS = (1 << 1),
}

export type bdev_specs = {
    fstype: string;	 /*!< Filesystem type */
    fssize: number;/*!< Filesystem size in bytes */
    zfs: { zfsroot: string }; /*!< ZFS root path */
    lvm: {
        vg: string;		/*!< LVM Volume Group name */
        lv: string;		/*!< LVM Logical Volume name */
        thinpool: string; /*!< LVM thin pool to use, if any */
    };
    dir: string; /*!< Directory path */
    rbd: {
        rbdname: string; /*!< RBD image name */
        rbdpool: string; /*!< Ceph pool name */
    }
}

export const Container: Container = <Container>bindings.Container;
type Container = {

    get name(): string;
    get state(): "STOPPED" | "STARTING" | "RUNNING" | "ABORTING" | "STOPPING";

    new(name: string): Container,

    start(useinit: number, argv: string[]): boolean;
    stop(): boolean;
    create(template: string, bdevtype: string, bdev_specs: Partial<bdev_specs>, flags: number, argv: string[]): boolean;
    getConfigItem(key: string): string;
    setConfigItem(key: string, value: string): boolean;
    clearConfigItem(key: string): boolean;
    clearConfig(): void;
}