import { Container } from "../../lib/types/container";

const defined = Container.List("defined").map(c => { return { name: c.name, state: c.state } });
console.log("Defined Containers:");
console.table(defined);

const active = Container.List("active").map(c => { return { name: c.name, state: c.state } });
console.log("Active Containers:");
console.table(active);

const all = Container.List("all").map(c => { return { name: c.name, state: c.state } });
console.log("Defined and or Active Containers:");
console.table(all);


