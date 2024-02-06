import EventEmitter = require("events");

export class ProgressPromise<T> extends Promise<T> {

    private events: EventEmitter = new EventEmitter();

    constructor(executor: (resolve: (value: T | PromiseLike<T>) => void, reject: (reason?: any) => void, emit: (event: string, data: any) => void) => void) {
        super((resolve, reject) => executor(resolve, reject, (event, data) => this.events.emit(event, data)));
    }

    on(event: string, callback: (data: T) => void) {
        return this.events.on(event, callback);
    }

    then<TResult1 = T, TResult2 = never>(onfulfilled?: ((value: T) => (PromiseLike<TResult1> | TResult1)) | undefined | null, onrejected?: ((reason: any) => (PromiseLike<TResult2> | TResult2)) | undefined | null): Promise<TResult1 | TResult2> {
        return super.then(onfulfilled, onrejected)
    }
}