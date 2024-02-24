//
// Created by A.P.A.Slaa on 2/21/24.
//

#ifndef NODE_LXC_ARRAY_H
#define NODE_LXC_ARRAY_H

#include "napi.h"
#include "./Undefined.h"

class Array {
public:
    static Napi::Value New(
            Napi::Env env, ///< N-API environment
            char **values) {
        auto array = Napi::Array::New(env);
        if (values != nullptr) {
            // Iterate over the array of strings obtained.
            for (int i = 0; values[i] != nullptr; ++i) {
                // Convert each string to a Napi::Object Type and add it to the Napi::Array.
                array[i] = Napi::String::New(env, values[i]);
            }
            // Free the memory allocated for the array of strings.
            free(values);
        }
        return array;
    }

    static char **NapiToCharStarArray(const Napi::Array &jsArray, uint32_t &length) {
        if (!jsArray) {
            length = 0;
            return new char *[]
                    {nullptr};
        }
        length = jsArray.Length();
        char **argv = new char *[length + 1]; // +1 for NULL terminator

        for (uint32_t i = 0; i < length; ++i) {
            std::string str = jsArray.Get(i).As<Napi::String>();
            argv[i] = new char[str.length() + 1];
            strcpy(argv[i], str.c_str());
        }
        argv[length] = nullptr; // NULL terminate the array

        return argv;
    }

    static char **NapiToCharStarArray(const Napi::Array &jsArray) {
        auto length = jsArray.Length();
        char **argv = new char *[length + 1]; // +1 for NULL terminator

        for (uint32_t i = 0; i < length; ++i) {
            std::string str = jsArray.Get(i).As<Napi::String>();
            argv[i] = new char[str.length() + 1];
            strcpy(argv[i], str.c_str());
        }
        argv[length] = nullptr; // NULL terminate the array

        return argv;
    }

    static void FreeCharStarArray(char **argv, uint32_t length) {
        if (!argv) {
            return;
        }
        for (uint32_t i = 0; i < length; ++i) {
            delete[] argv[i];
        }
        delete[] argv;
    }
};

#endif // NODE_LXC_ARRAY_H
