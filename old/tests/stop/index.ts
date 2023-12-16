//
// Created by alexander-slaa on 12/9/23.
//

import { Container } from "../../lib/types/container";

const containers = Container.List().filter(c => c.isRunning);

if (containers.length == 0) {
    console.warn("WARN:\tNo containers to stop");
    process.exit(0);
}

console.log(`${containers[0].name} CURRENT STATE: `, containers[0].state);
console.log(containers[0].stop());
console.log(`${containers[0].name} NEW STATE:`, containers[0].state);

