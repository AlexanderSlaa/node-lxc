import { readFileSync, readdirSync } from "fs";
import { LXC_CREATE, bdev_spec, bdev_specs, binding, helpers, LXC_ATTACH, External } from "../bindings";
import { join } from "path";
import { BDevType, LogLevel } from "./type";
import { Readable, Writable } from "node:stream";
import { LXC } from "../index";

export type ContainerCreateInput = {
    template: string,
    bdevtype: BDevType,
    bdev_spec: bdev_specs,
    verbosity?: number,
    args: string[]
};

export enum Personality {
    /**
     * Intel 32bit
     */
    X86 = 0x0008,
    /**
     * Intel 64bit
     */
    X86_64 = 0x0000
}

export type ContainerAttachOptions = ({
    /**
     * If ClearEnv is true the environment is cleared before running the command.
     */
    clear_env: true,
    /**
     * EnvToKeep specifies the environment of the process when ClearEnv is true.
     */
    extra_keep_env: string[]
} |
{
    /**
     * If ClearEnv is true the environment is cleared before running the command.
     */
    clear_env: false
}) &
{
    /**
     * Specify the namespaces to attach to, as OR'ed list of clone flags (syscall.CLONE_NEWNS | syscall.CLONE_NEWUTS ...).
     */
    namespaces: number,
    /**
     * Specify the architecture which the kernel should appear to be running as to the command executed.
     */
    personality: Personality | -1,
    /**
     * UID specifies the user id to run as.
     */
    uid: number,
    /**
     * GID specifies the group id to run as.
     */
    gid: number,
    /**
     * Groups specifies the list of additional group ids to run with.
     */
    groups: number[],
    /**
     * [stdinfd, stdoutfd, stderrfd]
     * stdinfd: specifies the fd to read input from.
     * stdoutdf: specifies the fd to write output to.
     * stderrfd: specifies the fd to write error output to.
     */
    stdio: [number, number, number],
    /**
     * Cwd specifies the working directory of the command.
     */
    cwd: string,
    /**
     * Env specifies the environment of the process.
     */
    extra_env_vars: string[],
    /**
     * RemountSysProc remounts /sys and /proc for the executed command.
     * This is required to reflect the container (PID) namespace context
     * if the command does not attach to the container's mount namespace.
     */
    remount_sys_proc: boolean
    /**
     * ElevatedPrivileges runs the command with elevated privileges.
     * The capabilities, cgroup and security module restrictions of the container are not applied.
     * @WARNING: This may leak privileges into the container.
     */
    elevated_privileges: boolean
}


export class Container {

    public static DEFAULT_CREATE: ContainerCreateInput = {
        template: "none",
        bdevtype: BDevType.dir,
        verbosity: LXC_CREATE.QUIET, //TODO this flag only works output from download script not redirected to nodejs
        bdev_spec: {},
        args: []
    };
    public static DEFAULT_ATTACH: ContainerAttachOptions = {
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
    }

    private _name: string
    private _$ref: External;


    private constructor(name: string) {
        this._name = name
    }

    get name() {
        return this._name;
    }

    set name(name: string) {
        if (!binding.lxc_rename(this._$ref, name)) {
            throw new Error("Unable to rename container");
        }
        this._name = name;
    }

    get isRunning() {
        return binding.lxc_running(this._$ref);
    }


    get isDefined() {
        return binding.lxc_defined(this._$ref);
    }

    get state() {
        return binding.lxc_state(this._$ref);
    }

    get isPrivileged() {
        return false;
        // return binding.lxc_get_config_item(this._$ref, "security.privileged") == "1";
    }

    get autoStart() {
        return binding.lxc_get_config_item(this._$ref, "lxc.start.auto") == "1";
    }

    get interfaces() {
        return binding.lxc_get_interfaces(this._$ref);
    }

    get config() {
        return readFileSync(this.configPath).toString().split("\n");
    }

    get configPath() {
        return join(binding.lxc_get_config_path(this._$ref), this._name, "config");
    }

    set logFile(filename: string) {
        if (helpers.VersionAtLeast(2, 1, 0)) {
            this.setConfigItem("lxc.log.file", filename);
        } else {
            this.setConfigItem("lxc.logfile", filename);
        }
    }

    get logFile() {
        if (helpers.VersionAtLeast(2, 1, 0)) {
            return this.getConfigItem("lxc.log.file");
        } else {
            return this.getConfigItem("lxc.logfile");
        }
    }


    set logLevel(level: LogLevel) {
        if (helpers.VersionAtLeast(2, 1, 0)) {
            this.setConfigItem("lxc.log.level", level.toString());
        } else {
            this.setConfigItem("lxc.loglevel", level.toString());
        }
    }

    get logLevel(): LogLevel {
        if (helpers.VersionAtLeast(2, 1, 0)) {
            return LogLevel[this.getConfigItem("lxc.log.level")];
        } else {
            return LogLevel[this.getConfigItem("lxc.loglevel")];
        }
    }

    get error() {
        return binding.lxc_error_str(this._$ref);
    }


    daemonize() {
        return binding.lxc_want_daemonize(this._$ref, true);
    }

    get configKeys() {
        return binding.lxc_get_keys(this._$ref, null);
    }

    setConfigItem(key: string, value: string, thorwOnFail: boolean = false) {
        const result = binding.lxc_set_config_item(this._$ref, key, value);
        if (thorwOnFail) {
            throw new Error(`Unable to set config item:${key}=${value}`);
        }
        return result;
    }

    getConfigItem(key: string, thorwOnFail: boolean = false) {
        const result = binding.lxc_get_config_item(this._$ref, key);
        if (thorwOnFail) {
            throw new Error(`Unable to get config item:${key}=?`);
        }
        return result;
    }

    stop() {
        return binding.lxc_stop(this._$ref);
    }

    start(useinit = 0, argv: string[] = []) {
        if (!this.isRunning) {
            return binding.lxc_start(this._$ref, useinit, argv)
        } else {
            throw new Error("Container already running");
        }
    }

    reboot() {
        return binding.lxc_reboot(this._$ref);
    }

    create(input: Partial<ContainerCreateInput>) { //TODO Only verbosity 1 works console output not redirected to node
        input = Object.assign(Container.DEFAULT_CREATE, input);
        if (!this.isDefined) {
            return binding.lxc_create(this._$ref, input.template, input.bdevtype, input.bdev_spec, input.verbosity, input.args);
        } else {
            throw new Error("Container already defined");
        }
    }

    private attachFlags(opt: ContainerAttachOptions) {
        const options = Object.assign({ attach_flags: LXC_ATTACH.DEFAULT }, opt);
        options.attach_flags = LXC_ATTACH.DEFAULT;

        if (options.remount_sys_proc) {
            options.attach_flags |= LXC_ATTACH.REMOUNT_PROC_SYS
        }

        if (options.elevated_privileges) {
            options.attach_flags &= ~(LXC_ATTACH.MOVE_TO_CGROUP | LXC_ATTACH.DROP_CAPABILITIES | LXC_ATTACH.LSM_EXEC)
        }
        return options
    }

    attachShell(options: Partial<ContainerAttachOptions>) {
        const opt = this.attachFlags(Object.assign(Container.DEFAULT_ATTACH, options))
        if (!this.isRunning) {
            throw new Error("Container not running");
        }
        return binding.lxc_attach(this._$ref, opt.clear_env, opt.namespaces, opt.personality, opt.uid, opt.gid, opt.groups, opt.stdio, opt.cwd, opt.extra_env_vars, opt.clear_env ? opt.extra_keep_env : [], opt.attach_flags);
    }

    static New(name: string, config_path?: string) {
        const container = new Container(name);
        container._$ref = binding.lxc_container_new(name, config_path)
        return container;
    }

    static List(filter: "defined" | "active" | "all" = "all", lxc_path?: string) {
        switch (filter) {
            case "defined": {
                return LXC.list_defined_containers(lxc_path);
            }
            case "active": {
                return LXC.list_active_containers(lxc_path);
            }
            /**
             * Listing all containers is default behavior for this function
             */
            default: {
                if (filter !== "all") {
                    console.warn('Unable to determine filter behavior returning all');
                }
                return LXC.list_all_containers(lxc_path)
            }
        }
    }
}
