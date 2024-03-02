import {Container} from "../../lib/bindings";

async function main() {
    const c = new Container("node-ct");
    const list = await c.snapshotList();
    console.table(list);
}

main().catch(console.error)