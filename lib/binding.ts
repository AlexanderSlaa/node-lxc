const _LXC = <_LXC>require('../build/Release/node-lxc');

type _LXC = {
    GetGlobalConfigItem(key: string): string | null;
    Version(): `${string}.${string}.${string}`;
    ContainerNames(lxc_path?: string): string[];
    CreateContainer(name: string): any;
}

export const LXC = {
    ..._LXC,
    DefaultConfigPath() {
        return this.GetGlobalConfigItem("lxc.lxcpath");
    },
    DefaultLvmVg() {
        return this.GetGlobalConfigItem("lxc.bdev.lvm.vg");
    },
    DefaultZfsRoot() {
        return this.GetGlobalConfigItem("lxc.bdev.zfs.root");
    },
    Containers(lxc_path?: string) {
        return _LXC.ContainerNames(lxc_path).map((value: string) => _LXC.CreateContainer(value));
    }
}

