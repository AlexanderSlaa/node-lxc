import { randomUUID } from "node:crypto";
import { LXC } from "../lib";
import { Container } from "../lib/types/container";
import { LogLevel } from "../lib/types/type";


const assert = require("assert");

assert(LXC, "The expected function is undefined");


let result = LXC.version;
console.log(result);
console.log(LXC.default_config_path());

const name = randomUUID();

//region create
const c = Container.New(name);
if (!c) {
    throw new Error("Failed to create container");
}

console.log("Creating container...\n")

c.logFile = `./${name}/` + c.name + ".log";
c.logLevel = LogLevel.DEBUG;


console.log(c.create({
    template: "download",
    args: ["--dist", "ubuntu", "--release", "jammy", "--arch", "amd64"]
}))



const stdio: [number, number, number] = [
    process.stdin.fd,
    process.stdout.fd,
    process.stderr.fd
]

console.log(c.attachShell({ stdio }));

//endregion
// console.log("Starting the container ....");
// if (!c.start()) {
//     throw new Error("Unable to start container");
// }


// const running = containers.filter((c) => c.isRunning);
// console.log(running[0].state);
// console.log(running[0].interfaces);
// console.log(running[0].stop());
