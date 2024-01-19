import {execFile, ExecOptions} from "child_process";
import {CommonOptions, DefaultOptionMap, OptionMapper} from "./index";

export type CheckpointOptions = {
    /**
     *  NAME of the container
     */
    name: string,
    /**
     * Restore container
     */
    restore?: true,
    /**
     * directory to save the checkpoint in
     */
    "checkpoint-dir"?: string,
    /**
     * Enable verbose criu logs
     */
    verbose?: true,
    /**
     * Path to criu action script
     */
    "action-script"?: string,
    /**
     *  Stop the container after checkpointing.
     */
    stop?: true,
    /**
     *  Only pre-dump the memory of the container.
     *  Container keeps on running and following checkpoints will only dump the changes.
     */
    "pre-dump"?: string
    /**
     * path to images from previous dump (relative to -D)
     */
    "predump-dir"?: string,
    /**
     * Daemonize the container (default)
     */
    daemon?: false,
    /**
     * Start with the current tty attached to /dev/console
     */
    foreground?: true,
    /**
     *  Load configuration file FILE
     */
    rcfile?: string
} & CommonOptions

export const checkpoint = (options: CheckpointOptions, execOptions: ExecOptions) => execFile("lxc-checkpoint", OptionMapper(DefaultOptionMap, options), execOptions);