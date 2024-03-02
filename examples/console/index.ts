import {Container} from "../../lib/bindings";

const name = "test-ct"

/**
 * In this example, a container is created and the calling process stdio is attached.
 */
async function main() {
    const c = new Container(name);
    if (c.defined && c.running) {
        await c.console(-1, [process.stdin.fd, process.stdout.fd, process.stderr.fd], 1);
    } else {
        console.warn(`Container '${name}' not defined or not running`);
    }
}

main().catch(console.error);