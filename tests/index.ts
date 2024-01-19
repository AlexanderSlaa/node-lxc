import {Container, LXC} from "../index";
import * as Path from "path";

console.log(`LXC version(${LXC.version})`);

const name = "node-ct"

const c = new Container(name);


async function main() {
    if (c.defined) {
        c.destroy();
    }
    await c.create({
        bdev: "dir",
        dir: Path.resolve(`./node-ct/rootfs`),
        template: "download",
        '--': ["--dist", "ubuntu", "--release", "lunar", "--arch", "amd64"]
    }).then((c) => c.start())
    console.log(c.state);
    c.attach({})
}

main().catch(console.error);

// createProcess.stdout.on("data", (buffer) => {
//     console.log(buffer.toString());
// })
//
// createProcess.stderr.on("data", (buffer) => {
//     console.error(buffer.toString());
// })
//
// createProcess.on("exit", () => {
//     console.log(c.state)
//
//     const ct = c.start();
//     ct.stdout.on("data", (buffer) => {
//         console.log(buffer.toString());
//     })
//
//     ct.on("exit", (code) => {
//         if(code == 1){
//             console.log("container started");
//         }
//     })
// })
//
