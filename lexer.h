#pragma once

#include "token.h"

#include <vector>

class Lexer
{
public:
    Lexer(const std::string&);
    std::vector<Token> lex();

private:
    std::string input;

};