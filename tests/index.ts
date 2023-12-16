import {Container, LXC_CREATE, LXC_LOGLEVEL} from "../index";

const c = new Container("test");

c.setConfigItem("lxc.log.file", "./test.log");
c.setConfigItem("lxc.log.level", LXC_LOGLEVEL.TRACE.toString());

console.log(c.name);

console.log("created", c.create("download", "dir", {}, LXC_CREATE.QUIET, ["--dist", "ubuntu", "--release", "jammy", "--arch", "amd64"]));

console.log("started", c.start(0, []));





