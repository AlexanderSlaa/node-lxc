//
// Created by alexander-slaa on 12/9/23.
//

import { Container } from "../../lib/types/container";
import { LogLevel } from "../../lib/types/type";

const containers = Container.List();

const stdio: [number, number, number] = [
    process.stdin.fd,
    process.stdout.fd,
    process.stderr.fd
]

if (!containers[0].isRunning) {
    console.log(containers[0].configPath)
    containers[0].logLevel = LogLevel.TRACE;
    containers[0].logFile = "./rubik.log";
    containers[0].daemonize();
    console.log(containers[0].start());
}

console.log(containers[0].attachShell({ stdio }));

console.log(containers[0].stop());

console.log(containers);
