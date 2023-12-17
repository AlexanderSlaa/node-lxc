"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var index_1 = require("../index");
console.log("LXC version(".concat(index_1.LXC.version, ")"));
var c = new index_1.Container("test");
c.setConfigItem("lxc.log.file", "./containers/test/.log");
c.setConfigItem("lxc.log.level", index_1.LXC_LOGLEVEL.TRACE.toString());
console.log(c.name);
console.log("created", c.create("download", "dir", {}, 1, ["--dist", "ubuntu", "--release", "jammy", "--arch", "amd64"]));
console.log("started", c.start(0, ["/sbin/init"]));
var attach_flags = index_1.LXC_ATTACH.DEFAULT;
c.attach(false, -1, -1, -1, -1, [], [process.stdin.fd, process.stdout.fd, process.stderr.fd], "/", [], [], attach_flags);
//# sourceMappingURL=index.js.map