const LXC = require("../lib");

const assert = require("assert");

assert(LXC, "The expected function is undefined");

debugger;

let result =  LXC.Version();
console.log(result);
result =  LXC.ContainerNames();
console.log(result);
result =  LXC.Containers();
console.log(result);