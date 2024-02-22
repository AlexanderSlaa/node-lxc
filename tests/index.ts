import {Container, LXC, LXC_CREATE, LXC_LOGLEVEL} from "../lib/bindings";

console.log(`LXC version(${LXC.GetVersion()})`);


async function main() {
    const name = "node-ct"

    const c = new Container(name);

    c.setConfigItem("lxc.log.file", `./${name}/.log`);
    c.setConfigItem("lxc.log.level", LXC_LOGLEVEL.TRACE.toString());

    // Setup network connection

    c.setConfigItem("lxc.net.0.type", "veth");
    c.setConfigItem("lxc.net.0.link", "lxcbr0");
    c.setConfigItem("lxc.net.0.flags", "up");
    c.setConfigItem("lxc.net.0.hwaddr", "00:16:3e:xx:xx:xx");


    // const clone = await c.clone({
    //     newname: "node-clone2",
    // });
    //
    // console.log(clone);
    //
    const keys = await c.getInterfaces();
    //
    const ips = await c.getIPs(keys[0], "inet");
    //
    console.log(keys, ips);
    //
    // console.log(c.name);
    //
    // c.daemonize = true;
    //
    // if (!c.defined) {
    //     console.log("created", await c.create("download", "dir", {}, LXC_CREATE.QUIET, ["--dist", "ubuntu", "--release", "lunar", "--arch", "amd64"]));
    //     console.log(process.pid);
    // }
    //
    // if (c.running) {
    //     await c.reboot(-1);
    // }else{
    //     console.log("started", await c.start(0, ["/sbin/init"]));
    //
    // }


    // console.log("started", await c.start(0, []));


    // const attach_flags = LXC_ATTACH.DEFAULT;


    // console.log("pid:", await c.attach(false, -1, -1, -1, -1, [], [process.stdin.fd, process.stdout.fd, process.stderr.fd], "/", [], [], attach_flags));
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
    //     ['cat', 'test.txt']
    // ));


}

main().catch(console.error);


