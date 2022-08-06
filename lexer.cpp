#include "lexer.h"

#include <cstring>


Lexer::Lexer(const std::string& input)
    : input(input)
{
    ;
}

std::vector<Token> Lexer::lex()
{
    std::vector<Token> tokens;
    int currentIdx = 0;

    while (currentIdx < input.length())
    {
        int tokenStartIdx = currentIdx;
        char lookahead = input[currentIdx];
        
        // White space or tab
        if (lookahead == ' ' || lookahead == '\t')
        {
            ++currentIdx;
        }
        else if (lookahead == ',')
        {
            ++currentIdx;
            // tokens.emplace_back(Token::Type::comma, ",", tokenStartIdx);
            tokens.push_back({ Token::Type::comma, ",", tokenStartIdx });
        }
        else if (lookahead == '[')
        {
            ++currentIdx;
            tokens.push_back({Token::Type::open_square, "[", tokenStartIdx});
        }
        else if (lookahead == ']')
        {
            ++currentIdx;
            tokens.push_back({Token::Type::close_square, "]", tokenStartIdx});
        }
        else if (lookahead == '(')
        {
            ++currentIdx;
            tokens.push_back({Token::Type::open_round, "(", tokenStartIdx});
        }
        else if (lookahead == ')')
        {
            ++currentIdx;
            tokens.push_back({Token::Type::close_round, ")", tokenStartIdx});
        }
        else if (lookahead == '-' && currentIdx + 1 < input.length() && input[currentIdx + 1] == '>')
        {
            currentIdx += 2;
            tokens.push_back({Token::Type::arrow, "->", tokenStartIdx});
        }
        else if (lookahead == '#') // Argument
        {
            std::string word;
            ++currentIdx;

            while (currentIdx < input.length() && isdigit(input[currentIdx]))
            {
                word += input[currentIdx];
                ++currentIdx;
            }

            tokens.push_back({Token::Type::arg, word, tokenStartIdx});
        }
        else if ((lookahead >= '0' && lookahead <= '9') || lookahead == '-') // Number
        {
            std::string word;
            bool decimal = false, empty = true;
            bool negative = lookahead == '-';

            if (negative)
            {
                word += '-';
                ++currentIdx;
            }

            while (currentIdx < input.length() && isdigit(input[currentIdx]))
            {
                empty = false;
                word += input[currentIdx];
                ++currentIdx;
            }

            if (currentIdx < input.length() && input[currentIdx] == '.')
            {
                decimal = true;
                word += input[currentIdx];
                ++currentIdx;
            }

            while (currentIdx < input.length() && isdigit(input[currentIdx]))
            {
                word += input[currentIdx];
                ++currentIdx;
            }

            if (empty)
            {
                throw "Lexer error while lexing a number\n";
            }

            if (decimal)
            {
                tokens.push_back({Token::Type::kw_double, word, tokenStartIdx});
            }
            else
            {
                tokens.push_back({Token::Type::kw_int, word, tokenStartIdx});
            }
        }
        else if (isalpha(lookahead) || input[currentIdx] == '_') // Identifier
        {
            std::string word;
            
            while (currentIdx < input.length() &&
                   (isalpha(input[currentIdx]) || isdigit(input[currentIdx] || input[currentIdx] == '_')))
            {
                word += input[currentIdx];
                ++currentIdx;
            }

            tokens.push_back({Token::Type::func, word, tokenStartIdx});
        }
        else
        {
            // TODO ERRORS
            throw "Unknown character";
        }
    }

    tokens.push_back({Token::Type::eof, "", currentIdx});

    return tokens;
}