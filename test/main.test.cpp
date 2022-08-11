#include "../lexer.h"
#include "../parser.h"
#include "../interpreter.h"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"


TEST_CASE("Default functions")
{
    GlobalScope globalScope;
    globalScope.loadDefaultLibrary();
    std::ifstream commands("commands.txt");
    std::ifstream results("results.txt");
    std::string line, res;
    
    if (commands.is_open() && results.is_open())
    {
        while (std::getline(commands, line) && std::getline(results, res))
        {
            Lexer l(line);
            std::vector<Token> tokens = l.lex();
            Parser p(tokens.begin());
            FunctionScope localScope(globalScope, nullptr, std::vector<std::shared_ptr<Node>>());
            std::shared_ptr<Value> val = p.parse(std::cout)->eval(localScope);

            REQUIRE(val->toString() == res);
        }

        commands.close();
        results.close();
    }
    else
    {
        REQUIRE(false);
    }
}