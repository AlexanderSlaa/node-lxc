import {Container} from "../../lib/bindings";

async function main() {
    const c = new Container("node-ct");
    if (c.defined) {
        console.log(await c.getInterfaces());
    }
}

main().catch(console.error)