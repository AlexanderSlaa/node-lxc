import { Container, LXC, LXC_ATTACH, LXC_CREATE, LXC_LOGLEVEL } from "../index";

console.log(`LXC version(${LXC.version})`);

const c = new Container("test");

c.setConfigItem("lxc.log.file", "./containers/test/.log");
c.setConfigItem("lxc.log.level", LXC_LOGLEVEL.TRACE.toString());

console.log(c.name);

// console.log("created", c.create("download", "dir", {}, 1, ["--dist", "ubuntu", "--release", "jammy", "--arch", "amd64"]));
console.log(process.pid);

// console.log("started", c.start(0, ["/sbin/init"]));
console.log("started", c.start());


const attach_flags = LXC_ATTACH.DEFAULT;

c.attach(false, -1, -1, -1, -1, [], [process.stdin.fd, process.stdout.fd, process.stderr.fd], "/", [], [], attach_flags);



