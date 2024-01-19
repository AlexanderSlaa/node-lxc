import {CommonOptions, OptionMapper} from "./index";
import {execFile, ExecOptions} from "child_process";

export type CopyOptions = {
    /**
     * NAME of the container
     */
    name: string,
    /**
     * NEWNAME for the restored container
     */
    newname?: string,
    /**
     * NEWPATH for the container to be stored
     */
    newpath?: string,
    /**
     * rename container
     */
    rename?: true,
    /**
     * create snapshot instead of clone
     */
    snapshot?: true,
    /**
     * allow snapshot creation even if source container is running
     */
    allowrunning?: true,
    /**
     * start with current tty attached to /dev/console
     */
    foreground?: true,
    /**
     *  daemonize the container (default)
     */
    daemon?: false,
    /**
     * directory to mount into container, either={bind,overlay}=/src-path or {bind,overlay}=/src-path:/dst-path
     */
    mount?: string,
    /**
     * backingstorage type for the container
     */
    backingstorage?: string,
    /**
     * place ephemeral container on a tmpfs (WARNING: On reboot all changes made to the container will be lost.)
     */
    tmpfs?: true
    /**
     * size of the new block device for block device containers
     */
    fssize?: string,

    /**
     * keep the hostname of the original container
     */
    keepname?: true,
    /**
     * arguments passed to the hook program
     */
    "--"?: string[],
    /**
     *  keep the MAC address of the original container
     */
    keepmac?: boolean,
    /**
     * Load configuration file FILE
     */
    rcfile?: string
} & ({
    /**
     * start ephemeral container
     */
    ephemeral: true,
    keepdata?: string
} | { ephemeral?: false }) & CommonOptions

export const CopyOptionsMapper = {
    "--": (key: string, value: string[]) => {
        return ["--", ...value];
    }
}

export const copy = (options: CopyOptions, execOptions: ExecOptions) => execFile("lxc-copy", OptionMapper(CopyOptionsMapper, options), execOptions);