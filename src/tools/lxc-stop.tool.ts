import {execFile, ExecOptions} from "child_process";
import {CommonOptions, DefaultOptionMap, OptionMapper} from "./index";

export type StopOptions = {
    /**
     * NAME of the container
     */
    name: string,
    /**
     *  reboot the container
     */
    reboot?: true,
    /**
     * don't wait for shutdown or reboot to complete
     */
    nowait?: true,
    /**
     *  wait T seconds before hard-stopping
     */
    timeout?: number,
    /**
     * kill container rather than request clean shutdown
     */
    kill?: true,
    /**
     * Avoid using API locks
     */
    nolock?: true,
    /**
     * Only request clean shutdown, don't force kill after timeout
     */
    nokill?: true,
    /**
     * Load configuration file FILE
     */
    rcfile?: string
} & CommonOptions

export const stop = (options: StopOptions, execOptions: ExecOptions) => execFile('lxc-stop', OptionMapper(DefaultOptionMap, options), execOptions);