import {Container, LXC} from "../index";
import {LogPriority} from "../src/tools";

console.log(`LXC version(${LXC.version})`);

const name = "node-ct"

const c = new Container(name);


async function main() {
    if (c.defined) {
        c.destroy();
    }
    const p = c.create({
        bdev: "dir",
        template: "download",
        logpriority: LogPriority.TRACE,
        logfile: "./node-ct/.log",
        '--': ["--dist", "ubuntu", "--release", "lunar", "--arch", "amd64"]
    });
    p.on("data", console.log);
    await p.then((c) => c.start({foreground: true}))
    console.log(c.state);
    c.attach({stdio: [process.stdin, process.stdout, process.stderr]})
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
