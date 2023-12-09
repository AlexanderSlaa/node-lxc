import { Container } from "../../lib/types/container";
import { BDevType, LogLevel } from "../../lib/types/type";

//TODO NOT WORKING!

const container = Container.New("test");

container.logLevel = LogLevel.TRACE;
container.logFile = "./test.log";

const created = container.create({
    template: "download",
    bdevtype: BDevType.dir,
    args: ["--dist", "ubuntu", "--release", "lunar", "--arch", "amd64"]
})

if (!created) {
    throw "Unable to create container";
}

console.log("Container created");



