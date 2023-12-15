//
// Created by alexander-slaa on 12/9/23.
//

import { Container } from "../../lib/types/container";

const containers = Container.List().filter(c => !c.isRunning);

if (containers.length == 0) {
    console.warn("WARN:\tNo containers to be started");
    process.exit(0);
}

console.log("CURRENT STATE: ", containers[0].state);
console.log(containers[0].start());
console.log("NEW STATE:", containers[0].state);

containers.forEach(c => console.log(c.name,c.state));
