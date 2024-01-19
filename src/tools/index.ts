export enum LogPriority {
    TRACE = "TRACE",
    DEBUG = "DEBUG",
    INFO = "INFO",
    NOTICE = "NOTICE",
    WARN = "WARN",
    ERROR = "ERROR",
    CRIT = "CRIT",
    ALERT = "ALERT",
    FATAL = "FATAL",
}

export type CommonOptions = {
    /**
     * Output log to FILE instead of stderr
     */
    logfile?: string,
    /**
     *  Set log priority to LEVEL
     */
    logpriority?: LogPriority,
    /**
     * Don't produce any output
     */
    quiet?: true,
    /**
     * Use specified container path
     */
    lxcpath?: string,
}

export const DefaultOptionMap = {
    $: (key: string, value: any) => {
        if(typeof value === "boolean" && value){
            return [`--${key}`];
        }else if(typeof value === "boolean" && value){
            //ignore output
        }
        return [`--${key}=${value}`];
    },
}

export type WithoutName<T> = T extends { name: string } ? Omit<T, 'name'> : T;


export function OptionMapper(map: Record<string, (key: string, value: any) => string[]>, values: Record<string, any>){
    map = Object.assign({...DefaultOptionMap}, map)
    const options: string[] = [];
    for (let valuesKey in values) {
        const mapper = valuesKey in map? map[valuesKey]: map['$'];
        if(mapper) {
            options.push(...mapper(valuesKey, values[valuesKey]));
        }else{
            throw "Incomplete mapper";
        }
    }
    return options.flat();
}
