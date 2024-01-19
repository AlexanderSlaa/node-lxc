import {CommonOptions, DefaultOptionMap, OptionMapper} from "./index";
import {execFile, ExecOptions} from "child_process";

export type SnapshotOptions = {
    /**
     * NAME of the container
     */
    name: string,
    //TODO: Change args on option selected
    /**
     * list all snapshots
     */
    list?: true,
    /**
     * restore snapshot NAME, e.g. 'snap0'
     */
    restore?: string,
    /**
     * NEWNAME for the restored container
     */
    newname?: string,
    /**
     * destroy snapshot NAME, e.g. 'snap0' use ALL to destroy all snapshots
     */
    destroy?: string | "ALL",
    /**
     * add FILE as a comment
     */
    comment?: string,
    /**
     * show snapshot comments
     */
    showcomments?: true,
    /**
     * Load configuration file FILE
     */
    rcfile?: string
} & CommonOptions;

export const snapshot = (options: SnapshotOptions, execOptions: ExecOptions) => execFile("lxc-snapshot", OptionMapper(DefaultOptionMap, options), execOptions);
