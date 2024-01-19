import {CommonOptions, OptionMapper} from "./index";
import {exec, execFile, ExecOptions} from "child_process";

export type BDEV_LVM = {
    /**
     * Use LVM lv name LVNAME
     * @default container name
     */
    bdev: 'lvm',
    /**
     * Use LVM vg called VG
     * @default lxc
     */
    lvm_name?: string,
    /**
     * Use LVM thin pool called TP
     * @default lxc
     */
    vgname?: string
}

export type BDEV_RBD = {
    /**
     *  Backing store type to use
     */
    bdev: "rbd"
    /**
     * Use Ceph RBD name RBDNAME
     * @default container name
     */
    rbdname?: string,
    /**
     * Use Ceph RBD pool name POOL
     * @default lxc
     */
    rbdpool?: string
}

export type BDEV_ZFS = {
    /**
     *  Backing store type to use
     */
    bdev: "zfs",
    /**
     *  Create zfs under given zfsroot
     *  @default tank/lxc
     */
    zfsroot?: string
}

export type BDEV_LVM_LOOP = {
    /**
     *  Backing store type to use
     */
    bdev: "lvm" | "loop",
    /**
     * Create fstype TYPE
     * @default ext4
     */
    fstype: string,
    /**
     * size SIZE * unit U (bBkKmMgGtT)
     * (Default: 1G, default unit: M)
     */
    fssize: `${number}${'b' | 'B' | 'k' | 'K' | 'm' | 'M' | 'g' | 'G' | 't' | 'T'}`
}

export type BDEV_DIR = {
    bdev: "dir",
    /**
     * Place rootfs directory under DIR
     */
    dir: string
}

export type CreateOptions = {
    /**
     * NAME of the container
     */
    name: string,
    /**
     * Initial configuration file
     */
    config?: string,
    /**
     *  Template to use to setup the container
     */
    template: string,
    /**
     * This will pass template-options to the template as arguments.
     * To see the list of options supported by the template, you can run lxc-create -t TEMPLATE -h.
     */
    "--": string[]

} & (BDEV_DIR | BDEV_LVM | BDEV_RBD | BDEV_ZFS | BDEV_LVM_LOOP) & CommonOptions


export const CreateOptionMapper = {
    "--": (key: string, value: string[]) => {
        return [key,...value];
    }
}

export const create = (options: CreateOptions, execOptions: ExecOptions) => execFile("lxc-create", OptionMapper(CreateOptionMapper, options), execOptions);