import {readFileSync, readdirSync} from "fs";
import {bdev_spec, bdev_specs, binding, helpers} from "../bindings";
import {join} from "path";
import {BackendStore, BackendStoreSpecs, bdev_type, LogLevel} from "./type";

export class Container {

    private _name: string
    private _$ref: any;

    private constructor(name: string) {
        this._name = name
    }

    get name() {
        return this._name;
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

    get interfaces() {
        return binding.lxc_get_interfaces(this._$ref);
    }


    get config() {
        const path = binding.lxc_get_config_path(this._$ref);
        return readFileSync(join(path, this._name, "config")).toString().split("\n");
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

    setConfigItem(key: string, value: string, thorwOnFail: boolean = false) {
        const result = binding.lxc_set_config_item(this._$ref, key, value);
        if (thorwOnFail) {
            throw new Error(`Unabled to set config item:${key}=${value}`);
        }
        return result;
    }

    getConfigItem(key: string, thorwOnFail: boolean = false) {
        const result = binding.lxc_get_config_item(this._$ref, key);
        if (thorwOnFail) {
            throw new Error(`Unabled to get config item:${key}=?`);
        }
        return result;
    }

    stop() {
        return binding.lxc_stop(this._$ref);
    }

    start(useinit: number = 1, ...argv: string[]) {
        if (!this.isRunning) {
            return binding.lxc_start(this._$ref, useinit, undefined)
        } else {
            throw new Error("Container already running");
        }
    }

    get error() {
        return binding.lxc_error_num(this._$ref);
    }

    reboot() {
        return binding.lxc_reboot(this._$ref);
    }

    create(template: string, bdevtype: bdev_type, bdev_spec: bdev_specs, verbosity: number = 0, args?: string[]) {
        if (!this.isDefined) {
            binding.lxc_create(this._$ref, template, bdevtype, bdev_spec, verbosity, args)
        }
    }


    private buildBdevSpecs

    static New(name: string, config_path?: string) {
        const container = new Container(name);
        container._$ref = binding.lxc_container_new(name, config_path)
        return container;
    }
}
