import {infoSync} from "../tools/lxc-info.tool";
import {startSync as LXCStart, type StartOptions} from "../tools/lxc-start.tool";
import {ChildProcess} from "child_process";
import {stop as LXCStop, type StopOptions} from "../tools/lxc-stop.tool";
import {create as LXCCreate, type CreateOptions} from "../tools/lxc-create.tool";
import {attach as LXCAttach, type AttachOptions} from "../tools/lxc-attach.tool";
import {CommonOptions, WithoutName} from "../tools";
import {ProgressPromise} from "./ProgressPromise";
import {CLIError} from "./CLIError";
import {destroySync as LXCDestroy, type DestroyOptions} from "../tools/lxc-destroy.tool";
import {Readable, Writable} from "stream";

export class Container {

    private readonly _name: string;
    private readonly _options: Exclude<CommonOptions, 'quite'> = {}
    private _configPath: string;

    get name(): string {
        return this._name
    }

    get state(): "STOPPED" | "STARTING" | "RUNNING" | "ABORTING" | "STOPPING" {
        if (!this.defined) {
            throw "Container not defined";
        }
        return infoSync({
            name: this.name,
            state: true,
            "no-humanize": true
        }, {stdio: [null, null, null]}) as "STOPPED" | "STARTING" | "RUNNING" | "ABORTING" | "STOPPING";
    }

    get defined(): boolean {
        try {
            infoSync({name: this.name, "no-humanize": true}, {stdio: [null, null, null]})
            return true;
        } catch (e) {
            return !e.stderr.toString().includes("doesn't exist")
        }
    }

    constructor(name: string, configPath?: string, options?: Exclude<CommonOptions, 'quite'>) {
        this._name = name;
        this._configPath = configPath;
        this._options = options;
    }

    start(startOptions?: WithoutName<StartOptions>): string {
        if (!this.defined) {
            throw "Container not defined";
        }
        return LXCStart({...startOptions, name: this.name, ...this._options}, {}).toString().trim();
    }

    // start():boolean;
    stop(options?: WithoutName<StopOptions>): ChildProcess {
        if (!this.defined) {
            throw "Container not defined";
        }
        return LXCStop({name: this.name, ...options, ...this._options}, {})
    }

    create(options: WithoutName<CreateOptions>): ProgressPromise<this> {
        if (this.defined) {
            throw "Container already defined";
        }
        return new ProgressPromise((resolve, reject, emit) => {
            const process = LXCCreate({name: this.name, ...options, ...this._options}, {});
            process.stdout.on("data", (buffer) => emit("data", buffer.toString()));
            const stdErr: Buffer[] = [];
            process.stderr.on("data", (buffer) => stdErr.push(buffer));
            process.on("error", (err) => reject(err))
            process.on("exit", (code) => {
                if (code == 0) {
                    resolve(this);
                } else {
                    reject(new CLIError(Buffer.concat(stdErr).toString(), process));
                }
            })
        })
    }

    // getConfigItem(key: string): string{
    //
    // }
    // setConfigItem(key: string, value: string): boolean;
    // clearConfigItem(key: string): boolean;
    // clearConfig(): void;

    attach(options: WithoutName<AttachOptions> & { stdio: [Writable | null, Readable | null, Readable | null] }) {
        if (!this.defined) {
            throw "Container not defined";
        }
        const process = LXCAttach({name: this.name, ...options, ...this._options}, {});
        process.stderr.pipe()
    }

    // exec(clear_env: string, namespace: number, personality: number, uid: null, guid: number, groups: number[], stdio: [number, number, number], cwd: string, env: string[], keep_env: string[], flags: number): number;

    // daemonize(v: boolean): boolean;
    destroy(options?: WithoutName<DestroyOptions>) {
        if (!this.defined) {
            throw "Container not defined";
        }
        return LXCDestroy({name: this.name, ...options, ...this._options}, {});
    }
}