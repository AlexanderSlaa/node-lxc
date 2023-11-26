export enum LogLevel {
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

export enum bdev_type {
    btrfs = "btrfs",
    dir = "dir",
    lvm = "lvm",
    zfs = "zfs",
    aufs = "aufs",
    overlayfs = "overlayfs",
    loop = "loop",
    best = "best"
}

export type TemplateOptions = {

    // Template specifies the name of the template.
    Template: string

    // Backend specifies the type of the backend.
    Backend: bdev_type

    BackendSpecs: BackendStoreSpecs

    // Distro specifies the name of the distribution.
    Distro: string

    // Release specifies the name/version of the distribution.
    Release: string

    // Arch specified the architecture of the container.
    Arch: string

    // Variant specifies the variant of the image (default: "default").
    Variant: string

    // Image server (default: "images.linuxcontainers.org").
    Server: string

    // GPG keyid (default: 0x...).
    KeyID: string

    // GPG keyserver to use.
    KeyServer: string

    // Disable GPG validation (not recommended).
    DisableGPGValidation: boolean

    // Flush the local copy (if present).
    FlushCache: boolean

    // Force the use of the local copy even if expired.
    ForceCache: boolean

    // ExtraArgs provides a way to specify template specific args.
    ExtraArgs: string[]
}


// BackendStoreSpecs represents a LXC storage backend.
export type BackendStoreSpecs = {
    FSType: string
    FSSize: number
    Dir: string
    ZFS: {
        Root: string
    }
    LVM: {
        VG: string,
        LV: string,
        Thinpool: string
    }
    RBD: {
        Name: string
        Pool: string
    }
}