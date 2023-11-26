import {LXC} from "../lib";
import {Container} from "../lib/types/container";
import {BackendStore, bdev_type, LogLevel} from "../lib/types/type";


const assert = require("assert");

assert(LXC, "The expected function is undefined");


let result = LXC.version;
console.log(result);

//region create

const c = Container.New("test");
if (!c) {
    throw new Error("Failed to create container");
}

console.log("Creating container...\n")

c.logFile = "./test_ct/" + c.name + ".log";
c.logLevel = LogLevel.DEBUG;

c.create("download", bdev_type.dir, {dir: "./test_ct/blkdev"})

//endregion
console.log("Starting the container ....");
if (!c.start()) {
    throw new Error("Unable to start container");
}


// const running = containers.filter((c) => c.isRunning);
// console.log(running[0].state);
// console.log(running[0].interfaces);
// console.log(running[0].stop());
