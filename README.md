## Development Prerequisites
1. Install NodeJS </br>
[Install NodeJS from distribution](https://github.com/nodesource/distributions#installation-instructions)
2. Installing NAPI build-tools
```shell
apt update -y && apt upgrade -y
```
```shell
apt install gcc g++ cmake
```
3. Installing LXC and the needed development files
```shell
apt install lxc lxc-dev -y
```
3.1. Checking LXC configuration
```shell
lxc-checkconfig
```
the resulting `lxc-checkconfig` will show something like
```text
LXC version 5.0.2
Kernel configuration not found at /proc/config.gz; searching...
Kernel configuration found at /boot/config-6.1.0-15-amd64

--- Namespaces ---
Namespaces: enabled
Utsname namespace: enabled
Ipc namespace: enabled
Pid namespace: enabled
User namespace: enabled
Network namespace: enabled

--- Control groups ---
Cgroups: enabled
Cgroup namespace: enabled
Cgroup v1 mount points:
Cgroup v2 mount points:
- /sys/fs/cgroup
  Cgroup device: enabled
  Cgroup sched: enabled
  Cgroup cpu account: enabled
  Cgroup memory controller: enabled
  Cgroup cpuset: enabled

--- Misc ---
Veth pair device: enabled, not loaded
Macvlan: enabled, not loaded
Vlan: enabled, not loaded
Bridges: enabled, loaded
Advanced netfilter: enabled, loaded
CONFIG_IP_NF_TARGET_MASQUERADE: enabled, not loaded
CONFIG_IP6_NF_TARGET_MASQUERADE: enabled, not loaded
CONFIG_NETFILTER_XT_TARGET_CHECKSUM: enabled, not loaded
CONFIG_NETFILTER_XT_MATCH_COMMENT: enabled, not loaded
FUSE (for use with lxcfs): enabled, loaded

--- Checkpoint/Restore ---
checkpoint restore: enabled
CONFIG_FHANDLE: enabled
CONFIG_EVENTFD: enabled
CONFIG_EPOLL: enabled
CONFIG_UNIX_DIAG: enabled
CONFIG_INET_DIAG: enabled
CONFIG_PACKET_DIAG: enabled
CONFIG_NETLINK_DIAG: enabled
File capabilities: enabled

Note : Before booting a new kernel, you can check its configuration
usage : CONFIG=/path/to/config /usr/bin/lxc-checkconfig
```

for lxc from source
```shell
  apt-get update -y && apt-get upgrade -y
  apt-get install -y pkg-config libseccomp-dev libssl-dev libselinux-dev libselinux-dev libapparmor-dev libcap-dev libsystemd-dev
```

```shell
meson setup -Dprefix=/usr build
meson compile -C build
```
