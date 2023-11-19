import {Container} from "./types/container";

const binding = <LXCBinding>require('../build/Release/node-lxc');


type LXCBinding = {
    /**
     * GlobalConfigItem returns the value of the given global config key.
     * @param key
     * @constructor
     */
    lxc_get_global_config_item(key: string): string | null;
    /**
     * Version returns the LXC version.
     */
    version(): `${string}.${string}.${string}`;
    version_object(): { major: number, minor: number, micro: number };
    is_devel(): boolean;
    /**
     * ContainerNames returns the names of defined and active containers on the system.
     * @param lxc_path
     * @constructor
     */
    list_all_containers(lxc_path?: string): string[];
    /**
     * DefinedContainerNames returns the names of the defined containers on the system.
     * @param lxc_path
     * @constructor
     */
    list_defined_containers(lxc_path?: string): string[];
    /**
     * ActiveContainerNames returns the names of the active containers on the system.
     * @param lxc_path
     * @constructor
     */
    list_active_containers(lxc_path?: string): string[];
    /**
     * NewContainer returns a new container external reference.
     * @param name
     * @param config
     */
    lxc_container_new(name: string, config?: string): {};
    /**
     * HasAPIExtension returns true if the extension is supported.
     * @param extension
     * @constructor
     */
    lxc_has_api_extension(extension: string): boolean;
    /**
     * IsSupportedConfigItem returns true if the key belongs to a supported config item.
     * @param key
     * @constructor
     */
    lxc_config_item_is_supported(key: string): boolean;
}
export const LXC = {
    ...binding,
    /**
     * DefaultConfigPath returns default config path.
     */
    DefaultConfigPath() {
        return binding.lxc_get_global_config_item("lxc.lxcpath");
    },
    /**
     * DefaultLvmVg returns the name of the default LVM volume group.
     */
    DefaultLvmVg() {
        return binding.lxc_get_global_config_item("lxc.bdev.lvm.vg");
    },
    /**
     * DefaultZfsRoot returns the name of the default ZFS root.
     */
    DefaultZfsRoot() {
        return binding.lxc_get_global_config_item("lxc.bdev.zfs.root");
    },
    /**
     * Containers returns the defined and active containers on the system. Only
     * containers that could retrieved successfully are returned.
     * @param lxc_path
     * @constructor
     */
    Containers(lxc_path?: string) {
        return binding.list_all_containers(lxc_path).map((value) => Container.NewContainer(value));
    },
    /**
     * DefinedContainers returns the defined containers on the system.  Only
     * containers that could retrieved successfully are returned.
     * Caller needs to call Release() on the returned containers to release resource
     * @param lxc_path
     * @constructor
     */
    DefinedContainers(lxc_path?: string) {
        return binding.list_defined_containers(lxc_path).map((value) => Container.NewContainer(value));
    },
    /**
     * ActiveContainers returns the active containers on the system.
     * Only containers that could retrieved successfully are returned.
     * @param lxc_path
     * @constructor
     */
    ActiveContainers(lxc_path?: string) {
        return binding.list_active_containers(lxc_path).map((value) => Container.NewContainer(value));
    },
    /** VersionAtLeast returns true when the tested version >= current version.
     *
     * @param major
     * @param minor
     * @param micro
     * @constructor
     */
    VersionAtLeast(major: number, minor: number, micro: number) {
        if (binding.is_devel()) {
            return true
        }
        const versionInfo = binding.version_object();
        if (major > versionInfo.major) {
            return false
        }
        if (major == versionInfo.major && minor > versionInfo.minor) {
            return false
        }
        if (major == versionInfo.major && minor == versionInfo.minor && micro > versionInfo.micro) {
            return false
        }
        return true
    },
}

