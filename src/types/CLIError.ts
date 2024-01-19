import {ChildProcess} from "child_process";

export class CLIError extends Error {

    private _childProcess: ChildProcess;

    constructor(message: string, childProcess: ChildProcess) {
        super(message);
        this._childProcess = childProcess;
    }

    get childProcess(){
        return this._childProcess;
    }

}