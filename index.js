"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.LXC = exports.Container = exports.Personality = exports.DEFAULT_ATTACH = exports.LXC_ATTACH = exports.LXC_CREATE = exports.LXC_LOGLEVEL = void 0;
var bindings;
if (process.env["MODE"] === "DEV") {
    bindings = require("./build/Debug/node-lxc.node");
}
else {
    bindings = require("./build/Release/node-lxc.node");
}
//region consts
var LXC_LOGLEVEL;
(function (LXC_LOGLEVEL) {
    LXC_LOGLEVEL["TRACE"] = "TRACE";
    LXC_LOGLEVEL["DEBUG"] = "DEBUG";
    LXC_LOGLEVEL["INFO"] = "INFO";
    LXC_LOGLEVEL["NOTICE"] = "NOTICE";
    LXC_LOGLEVEL["WARN"] = "WARN";
    LXC_LOGLEVEL["ERROR"] = "ERROR";
    LXC_LOGLEVEL["CRIT"] = "CRIT";
    LXC_LOGLEVEL["ALERT"] = "ALERT";
    LXC_LOGLEVEL["FATAL"] = "FATAL";
})(LXC_LOGLEVEL || (exports.LXC_LOGLEVEL = LXC_LOGLEVEL = {}));
var LXC_CREATE;
(function (LXC_CREATE) {
    LXC_CREATE[LXC_CREATE["QUIET"] = 1] = "QUIET";
    LXC_CREATE[LXC_CREATE["MAXFLAGS"] = 2] = "MAXFLAGS";
})(LXC_CREATE || (exports.LXC_CREATE = LXC_CREATE = {}));
var LXC_ATTACH;
(function (LXC_ATTACH) {
    /**
     * Move to cgroup
     * @default on
     */
    LXC_ATTACH[LXC_ATTACH["MOVE_TO_CGROUP"] = 1] = "MOVE_TO_CGROUP";
    /**
     * Drop capabilities
     * @default on
     */
    LXC_ATTACH[LXC_ATTACH["DROP_CAPABILITIES"] = 2] = "DROP_CAPABILITIES";
    /**
     * Set personality
     * @default on
     */
    LXC_ATTACH[LXC_ATTACH["SET_PERSONALITY"] = 4] = "SET_PERSONALITY";
    /**
     * Execute under a Linux Security Module
     * @default on
     */
    LXC_ATTACH[LXC_ATTACH["LSM_EXEC"] = 8] = "LSM_EXEC";
    /**
     * Remount /proc filesystem
     * @default off
     */
    LXC_ATTACH[LXC_ATTACH["REMOUNT_PROC_SYS"] = 65536] = "REMOUNT_PROC_SYS";
    /**
     * TODO: currently unused
     * @default off
     */
    LXC_ATTACH[LXC_ATTACH["LSM_NOW"] = 131072] = "LSM_NOW";
    /**
     * PR_SET_NO_NEW_PRIVS
     * Set PR_SET_NO_NEW_PRIVS to block execve() gainable privileges.
     *
     * @default off
     *
     */
    LXC_ATTACH[LXC_ATTACH["NO_NEW_PRIVS"] = 262144] = "NO_NEW_PRIVS";
    /**
     * Allocate new terminal for attached process.
     * @default off
     */
    LXC_ATTACH[LXC_ATTACH["TERMINAL"] = 524288] = "TERMINAL";
    /**
     * Set custom LSM label specified in @lsm_label.
     * @default off
     */
    LXC_ATTACH[LXC_ATTACH["LSM_LABEL"] = 1048576] = "LSM_LABEL";
    /**
     * Set additional group ids specified in @groups.
     * @default off
     */
    LXC_ATTACH[LXC_ATTACH["SETGROUPS"] = 2097152] = "SETGROUPS";
    /**
     * We have 16 bits for things that are on by default and 16 bits that
     * are off by default, that should be sufficient to keep binary
     * compatibility for a while
     *
     * Mask of flags to apply by default
     */
    LXC_ATTACH[LXC_ATTACH["DEFAULT"] = 65535] = "DEFAULT";
})(LXC_ATTACH || (exports.LXC_ATTACH = LXC_ATTACH = {}));
exports.DEFAULT_ATTACH = {
    namespaces: -1,
    personality: -1,
    cwd: "/",
    uid: -1,
    gid: -1,
    groups: [],
    clear_env: false,
    extra_env_vars: [],
    stdio: [process.stdin.fd, process.stdout.fd, process.stderr.fd],
    remount_sys_proc: false,
    elevated_privileges: false
};
//endregion
//region types
var Personality;
(function (Personality) {
    /**
     * Intel 32bit
     */
    Personality[Personality["X86"] = 8] = "X86";
    /**
     * Intel 64bit
     */
    Personality[Personality["X86_64"] = 0] = "X86_64";
})(Personality || (exports.Personality = Personality = {}));
//endregion
//region type bindings
exports.Container = bindings.Container;
exports.LXC = {
    get version() {
        return bindings.GetVersion();
    },
    getGlobalConfigItem: function (key) {
        return bindings.getGlobalConfigItem(key);
    },
    get listAllContainerNames() {
        return bindings.ListAllContainers();
    },
    get listAllDefinedContainerNames() {
        return bindings.ListAllDefinedContainers();
    },
    get listAllActiveContainerNames() {
        return bindings.ListAllActiveContainers();
    }
};
//# sourceMappingURL=index.js.map