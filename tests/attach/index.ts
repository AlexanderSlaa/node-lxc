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

const rubik = containers.find((c) => c.name == "rubik")

if (!rubik.isRunning) {
    console.log(rubik.configPath)
    rubik.logLevel = LogLevel.TRACE;
    rubik.logFile = "./rubik.log";
    console.log(rubik.daemonize());
    console.log(rubik.start());
    console.log(rubik.error);
}

console.log(rubik.attachShell({ stdio }));

// console.log(containers[0].stop());

console.log(containers);
