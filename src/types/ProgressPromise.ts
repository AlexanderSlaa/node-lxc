import EventEmitter = require("events");

export class ProgressPromise<T> extends Promise<T>{

    private events: EventEmitter = new EventEmitter();

    constructor(executor: (resolve: (value: T | PromiseLike<T>) => void, reject: (reason?: any) => void, emit: (event: string,data: any) => void) => void) {
        super((resolve, reject) => executor(resolve, reject, (event, data) => this.events.emit(event,data)));
    }

}