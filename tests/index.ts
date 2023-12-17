import { randomUUID } from "crypto";
import {Container, LXC, LXC_ATTACH, LXC_CREATE, LXC_LOGLEVEL} from "../index";
import { mkdirSync } from "fs";
import { resolve } from "path";

console.log(`LXC version(${LXC.version})`);

const name = randomUUID();

const c = new Container(name);

c.setConfigItem("lxc.log.file", `./containers/${name}/.log`);
c.setConfigItem("lxc.log.level", LXC_LOGLEVEL.TRACE.toString());

console.log(c.name);

const path = resolve(`./containers/${name}/rootfs`);

mkdirSync(path);

console.log("created", c.create("download", "dir", {dir: path}, 1, ["--dist", "ubuntu", "--release", "lunar", "--arch", "amd64"]));
console.log(process.pid);

c.daemonize(true);

console.log("started", c.start(0, ["/sbin/init"]));
// console.log("started", c.start());



const attach_flags = LXC_ATTACH.DEFAULT;

c.attach(false, -1, -1, -1, -1, [], [process.stdin.fd, process.stdout.fd, process.stderr.fd], "/", [], [], attach_flags);



