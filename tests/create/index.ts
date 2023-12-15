import { Container } from "../../lib/types/container";
import { BDevType, LogLevel } from "../../lib/types/type";

//TODO NOT WORKING!

const container = Container.New("rubik");

container.logLevel = LogLevel.TRACE;
container.logFile = "./rubik.log";

const created = container.create({
    template: "download",
    bdevtype: BDevType.dir,
    args: ["--dist", "ubuntu", "--release", "jammy", "--arch", "amd64"]
})

if (!created) {
    throw "Unable to create container";
}

console.log("Container created");



