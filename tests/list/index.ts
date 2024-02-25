import {LXC} from "../../lib/bindings";

async function main() {
    LXC.ListAllContainers().forEach(console.log)
}

main().catch(console.error)