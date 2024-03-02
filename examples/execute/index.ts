import {Container} from "../../lib/bindings";

async function main() {
    const c = new Container("node-ct");
    if (c.defined && c.running) {
        await c.exec({argv: ['uname', '-a']})
    }
}

main().catch(console.error)