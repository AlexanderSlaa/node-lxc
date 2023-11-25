import {binding} from "../bindings";

export class Container {

    private name: string
    private $ref: any;

    private constructor(name: string) {
        this.name = name
    }

    get isRunning() {
        return binding.lxc_running(this.$ref);
    }

    get state() {
        return binding.lxc_state(this.$ref);
    }

    get interfaces() {
        return binding.lxc_get_interfaces(this.$ref);
    }

    stop() {
        return binding.lxc_stop(this.$ref);
    }

    start(useinit: number = 0, ...argv: string[]) {
        return binding.lxc_start(this.$ref, useinit, ...argv)
    }

    reboot() {
        return binding.lxc_reboot(this.$ref);
    }


    static New(name: string, config_path?: string) {
        const container = new Container(name);
        container.$ref = binding.lxc_container_new(name, config_path)
        return container;
    }
}
