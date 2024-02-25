import {Container} from "../../lib/bindings";

async function main() {
    const c = new Container("node-ct");
    if(!c.defined){
        throw "Container not defined"
    }
    c.reboot().then(()=> console.log("Container stopped"))
}

main().catch(console.error)