#pragma once

#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

//! Singleton class for the interpreter
class ListFunc
{
public:
    ListFunc(const ListFunc& other) = delete;
    ListFunc& operator=(const ListFunc& other) = delete;

    //! Returns static instance of the singleton object
    static ListFunc& getInstance()
    {
        static ListFunc object;

        return object;
    }

    int run();
    int run(const char* path);

private:
    GlobalScope globalScope;

    //! Loads default library
    ListFunc()
    {
        globalScope.loadDefaultLibrary();
    }

};