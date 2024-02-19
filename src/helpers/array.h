//
// Created by root on 2/11/24.
//

#ifndef NODE_LXC_ARRAY_H
#define NODE_LXC_ARRAY_H

#include <napi.h>

char ** napiArrayToCharStarArray(const Napi::Array& jsArray) {
    uint32_t length = jsArray.Length();
    char ** argv = new char *[length + 1]; // +1 for NULL terminator

    for (uint32_t i = 0; i < length; ++i) {
        std::string str = jsArray.Get(i).As<Napi::String>();
        argv[i] = new char[str.length() + 1];
        strcpy(argv[i], str.c_str());
    }
    argv[length] = nullptr; // NULL terminate the array

    return argv;
}

void freeCharStarArray(char ** argv, uint32_t length) {
    for (uint32_t i = 0; i < length; ++i) {
        delete[] argv[i];
    }
    delete[] argv;
}
#endif //NODE_LXC_ARRAY_H
