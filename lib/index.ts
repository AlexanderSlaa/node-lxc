import {Container} from "./types/container";
import {binding} from "./bindings";

export const LXC = {
    //region direct from C bindings
    version: binding.version,
    get_global_config_item: binding.lxc_get_global_config_item,
    //endregion
    /**
     * default_config_path returns default config path.
     */
    default_config_path() {
        return binding.lxc_get_global_config_item("lxc.lxcpath");
    },
    /**
     * default_lvm_vg returns the name of the default LVM volume group.
     */
    default_lvm_vg() {
        return binding.lxc_get_global_config_item("lxc.bdev.lvm.vg");
    },
    /**
     * default_zfs_vg returns the name of the default ZFS root.
     */
    default_zfs_vg() {
        return binding.lxc_get_global_config_item("lxc.bdev.zfs.root");
    },
    /**
     * Containers returns the defined and active containers on the system. Only
     * containers that could retrieved successfully are returned.
     * @param lxc_path
     * @constructor
     */
    list_all_containers(lxc_path?: string) {
        return binding.list_all_containers(lxc_path).map((value) => Container.New(value));
    },
    /**
     * DefinedContainers returns the defined containers on the system.  Only
     * containers that could retrieved successfully are returned.
     * Caller needs to call Release() on the returned containers to release resource
     * @param lxc_path
     * @constructor
     */
    list_defined_containers(lxc_path?: string) {
        return binding.list_defined_containers(lxc_path).map((value) => Container.New(value));
    },
    /**
     * ActiveContainers returns the active containers on the system.
     * Only containers that could retrieved successfully are returned.
     * @param lxc_path
     * @constructor
     */
    list_active_containers(lxc_path?: string) {
        return binding.list_active_containers(lxc_path).map((value) => Container.New(value));
    },

}

