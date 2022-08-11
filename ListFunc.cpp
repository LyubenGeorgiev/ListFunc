#include "ListFunc.h"

#include <fstream>


int ListFunc::run()
{
    std::string line;

    for (;;)
    {
        std::getline(std::cin, line);
        if (std::cin.eof() || line == "exit")
        {
            break;
        }
        else if (line.empty())
        {
            continue;
        }

        try
        {
            Lexer lexer(line);
            std::vector<Token> tokens = lexer.lex();

            Parser parser(tokens.begin());
            FunctionScope localScope(globalScope, nullptr, std::vector<std::shared_ptr<Node>>());
            std::shared_ptr<Value> val = parser.parse(std::cout)->eval(localScope);

            if (val)
            {
                std::cout << "> " << val->toString() << '\n';
            }
        }
        catch (const std::runtime_error &execException)
        {
            std::cerr << execException.what() << std::endl;
            continue;
        }
        catch (...)
        {
            return -1;
        }
    }

    return 0;
}

int ListFunc::run(const char* path)
{
    std::string line;

    std::ifstream file(path);
    if (file.is_open())
    {
        while (std::getline(file,line))
        {
            std::cout << line << '\n';
            if (line == "exit")
            {
                break;
            }
            else if (line.empty())
            {
                continue;
            }

            try
            {
                Lexer lexer(line);
                std::vector<Token> tokens = lexer.lex();

                Parser parser(tokens.begin());
                FunctionScope localScope(globalScope, nullptr, std::vector<std::shared_ptr<Node>>());
                std::shared_ptr<Value> val = parser.parse(std::cout)->eval(localScope);

                if (val)
                {
                    std::cout << "> " << val->toString() << '\n';
                }
            }
            catch (const std::runtime_error &execException)
            {
                std::cerr << execException.what() << std::endl;
                continue;
            }
            catch (...)
            {
                return -1;
            }
        }
        file.close();

        return run();
    }

    std::cout << "Problem while opening file!\n";

    return run();
}