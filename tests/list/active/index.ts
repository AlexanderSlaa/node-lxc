import {LXC} from "../../../lib/bindings";

async function main() {
    LXC.ListAllActiveContainers().forEach(console.log)
}

main().catch(console.error)