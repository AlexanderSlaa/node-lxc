import { Container, LXC, LXC_ATTACH, LXC_CREATE, LXC_LOGLEVEL } from "../index";

console.log(`LXC version(${LXC.version})`);

const name = "node-ct"

const c = new Container(name);

c.setConfigItem("lxc.log.file", `./${name}/.log`);
c.setConfigItem("lxc.log.level", LXC_LOGLEVEL.TRACE.toString());

c.setConfigItem("lxc.idmap", "u 0 100000 65536")
c.setConfigItem("lxc.idmap", "g 0 100000 65536")
c.setConfigItem("lxc.monitor.unshare", "1")

console.log(c.name);

c.daemonize(true);

if (!c.defined) {
    console.log("created", c.create("download", "dir", {}, LXC_CREATE.QUIET, ["--dist", "ubuntu", "--release", "lunar", "--arch", "amd64"]));
    console.log(process.pid);
}


console.log("started", c.start(0, ["/sbin/init"]));
// console.log("started", c.start());



const attach_flags = LXC_ATTACH.DEFAULT;

c.attach(false, -1, -1, -1, -1, [], [process.stdin.fd, process.stdout.fd, process.stderr.fd], "/", [], [], attach_flags);



