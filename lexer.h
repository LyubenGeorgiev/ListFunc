#pragma once

#include "token.h"

#include <vector>

//! Lexing string into vector of Tokens
class Lexer
{
public:
    Lexer(const std::string&);

    //! Converts the internal string to list of Tokens
    std::vector<Token> lex();

private:
    std::string input;

};