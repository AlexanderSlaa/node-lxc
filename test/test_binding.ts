import { create } from "domain";
import { LXC } from "../lib";
import { Container } from "../lib/types/container";
import { bdev_type, LogLevel } from "../lib/types/type";
import { LXC_CREATE } from "../lib/bindings";


const assert = require("assert");

assert(LXC, "The expected function is undefined");


let result = LXC.version;
console.log(result);
console.log(LXC.default_config_path());

//region create
const c = Container.New("test12");
if (!c) {
    throw new Error("Failed to create container");
}

console.log("Creating container...\n")

c.logFile = "./test_ct/" + c.name + ".log";
c.logLevel = LogLevel.DEBUG;

// process.stderr.on("data", (d) => {
//     console.log(d.toString());
// })

// process.stdout.on("data", (d) => {
//     console.log(d.toString());
// })

// process.stdin.on("data", (d) => {
//     console.log(d.toString());
// })

console.log(c.create("download", bdev_type.dir, {}, 0, ["--dist", "ubuntu", "--release", "lunar", "--arch", "amd64"]))
console.error(c.error);

//endregion
// console.log("Starting the container ....");
// if (!c.start()) {
//     throw new Error("Unable to start container");
// }


// const running = containers.filter((c) => c.isRunning);
// console.log(running[0].state);
// console.log(running[0].interfaces);
// console.log(running[0].stop());
