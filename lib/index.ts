
//LXC only runs on linux machines
if (process.platform === "win32") {
    throw new Error("This library isn't supported on Windows Systems");
}

//TODO: Add mapping for binding from C++
