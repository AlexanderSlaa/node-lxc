export enum LXC_MOUNT{
    V1 = 0
}

export type lxc_mount = {
    /**
     * only value LXC_MOUNT_API_V1 = 1
     */
    version: LXC_MOUNT
}