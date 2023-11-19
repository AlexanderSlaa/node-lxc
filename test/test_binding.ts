import {LXC} from "../lib/binding";

const assert = require("assert");

assert(LXC, "The expected function is undefined");

let result: any =  LXC.Version();
console.log(result);
result =  LXC.ContainerNames();
console.log(result);
result =  LXC.Containers();
console.log(result);