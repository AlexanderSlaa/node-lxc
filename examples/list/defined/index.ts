import {LXC} from "../../../lib/bindings";

async function main() {
    LXC.ListAllDefinedContainer().forEach(console.log)
}

main().catch(console.error)