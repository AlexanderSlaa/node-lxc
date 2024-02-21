import {LXC_CLONE} from "../constants";

export type lxc_clone_options =
    (
        {
            /**
             * New name for the container.
             */
            newname: string
            /**
             * lxcpath in which to create the new container
             * If undefined, the original container's lxcpath will be used.
             * (XXX: should we use the default instead?)
             */
            lxcpath?: string
        } |
        {
            /**
             * New name for the container.
             * If undefined, the same name is used and a new lxcpath MUST be specified.
             */
            newname?: string
            /**
             * lxcpath in which to create the new container
             */
            lxcpath: string
        }
        ) & { flags?: number | LXC_CLONE, bdevtype?: string, bdevdata?: string, newsize?: number, hookargs?: string[] };