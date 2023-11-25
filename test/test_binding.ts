const {LXC} = require("../lib");

const assert = require("assert");

assert(LXC, "The expected function is undefined");

debugger;

let result = LXC.version;
console.log(result);
const containers = LXC.list_all_containers();
const running = containers.filter((c) => c.isRunning);
console.log(running[0].state);
console.log(running[0].interfaces);
console.log(running[0].stop());
