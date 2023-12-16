import {Container, LXC_CREATE} from "../index";

const c = new Container("test");

console.log(c.name);

//
// console.log("created", c.create("download", "dir", {}, LXC_CREATE.QUIET, ["--dist", "ubuntu", "--release", "jammy", "--arch", "amd64"]));
//
// console.log("started", c.start(0, []));
//
//



