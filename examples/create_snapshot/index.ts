import {Container} from "../../lib/bindings";

async function main() {
    const c = new Container("node-ct");
    if (c.defined) {
        await c.snapshot("./test") //TODO: Check if this works
    }
}

main().catch(console.error)