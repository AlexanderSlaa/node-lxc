import {Container, LXC_CREATE} from "../../lib/bindings";

async function main() {
    const c = new Container("node-ct");
    if (!c.defined) {
        console.log("Container creating...")
        await c.create({template: "download", argv: ["--dist", "ubuntu", "--release", "lunar", "--arch", "amd64"]});
        console.log("Done")
    }
}

main().catch(console.error)