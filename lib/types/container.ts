import {LXC} from "../index";

export class Container {

    private $ref: any;

    static NewContainer(name: string) {
        const container = new Container();
        // container.$ref = LXC.NewContainer(name);
        return container;
    }

}