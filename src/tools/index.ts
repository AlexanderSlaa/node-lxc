export enum LogPriority {

}

export type CommonOptions = {
    logfile?: string,
    logpriority?: LogPriority,
    quiet?: true,
    lxcpath?: string,
}

export function OptionMapper(map: Record<string, (key: string, value: any) => string[]>, values: Record<string, any>){
    const options: string[] = [];
    for (let valuesKey in values) {
        const mapper = valuesKey in map? map[valuesKey]: map['$'];
        if(mapper) {
            options.push(...map[valuesKey](valuesKey, valuesKey[valuesKey]));
        }else{
            throw "Incomplete mapper";
        }
    }
    return options;
}
