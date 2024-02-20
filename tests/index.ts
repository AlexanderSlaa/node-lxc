import {Container, LXC, LXC_ATTACH, LXC_CREATE, LXC_LOGLEVEL} from "../index";
import {openSync} from "fs";

console.log(`LXC version(${LXC.version})`);


async function main() {
    const name = "node-ct2"

    const c = new Container(name);

    c.setConfigItem("lxc.log.file", `./${name}/.log`);
    c.setConfigItem("lxc.log.level", LXC_LOGLEVEL.TRACE.toString());

    // Setup network connection

    c.setConfigItem("lxc.net.0.type", "veth");
    c.setConfigItem("lxc.net.0.link", "lxcbr0");
    c.setConfigItem("lxc.net.0.flags", "up");
    c.setConfigItem("lxc.net.0.hwaddr", "00:16:3e:xx:xx:xx");

    console.log(c.name);

    c.daemonize(true);

    if (!c.defined) {
        console.log("created", await c.create("download", "dir", {}, LXC_CREATE.QUIET, ["--dist", "ubuntu", "--release", "lunar", "--arch", "amd64"]));
        console.log(process.pid);
    }


    console.log("started", await c.start(0, ["/sbin/init"]));


    // console.log("started", await c.start(0, []));


    const attach_flags = LXC_ATTACH.DEFAULT;

    console.log("pid:", await c.attach(false, -1, -1, -1, -1, [], [process.stdin.fd, process.stdout.fd, process.stderr.fd], "/", [], [], attach_flags));
    // console.log("pid:", await c.exec(
    //     false,
    //     -1,
    //     -1,
    //     -1,
    //     -1,
    //     [],
    //     [process.stdin.fd, process.stdout.fd, process.stderr.fd],
    //     "/",
    //     [],
    //     [],
    //     attach_flags,
    //     ['echo', 'helloworld', '|', 'test.txt']
    // ));
    console.log("pid:", await c.exec(
        false,
        -1,
        -1,
        -1,
        -1,
        [],
        [process.stdin.fd, process.stdout.fd, process.stderr.fd],
        "/",
        [],
        [],
        attach_flags,
        ['cat', 'test.txt']
    ));


}

main().catch(console.error);


