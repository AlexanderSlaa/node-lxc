//
// Created by alexander-slaa on 12/9/23.
//

import { Container } from "../../lib/types/container";

const containers = Container.List();

if (containers.length == 0) {
    console.error("No containers defined");
}

console.log(containers[0].configKeys); //TODO UNTESTED