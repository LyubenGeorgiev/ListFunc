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
            tokens.push_back({ Token::Type::COMMA, ",", tokenStartIdx });
        }
        else if (lookahead == '[')
        {
            ++currentIdx;
            tokens.push_back({Token::Type::OPEN_SQUARE, "[", tokenStartIdx});
        }
        else if (lookahead == ']')
        {
            ++currentIdx;
            tokens.push_back({Token::Type::CLOSE_SQUARE, "]", tokenStartIdx});
        }
        else if (lookahead == '(')
        {
            ++currentIdx;
            tokens.push_back({Token::Type::OPEN_ROUND, "(", tokenStartIdx});
        }
        else if (lookahead == ')')
        {
            ++currentIdx;
            tokens.push_back({Token::Type::CLOSE_ROUND, ")", tokenStartIdx});
        }
        else if (lookahead == '-' && currentIdx + 1 < input.length() && input[currentIdx + 1] == '>')
        {
            currentIdx += 2;
            tokens.push_back({Token::Type::ARROW, "->", tokenStartIdx});
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

            tokens.push_back({Token::Type::ARG, word, tokenStartIdx});
        }
        else if ((lookahead >= '0' && lookahead <= '9') || lookahead == '-' || lookahead == '+') // Number
        {
            std::string word;
            bool decimal = false, empty = true;

            if (lookahead == '-' || lookahead == '+')
            {
                word += input[currentIdx];
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
                throw std::runtime_error("Lexer error while lexing a number");
            }

            if (decimal)
            {
                tokens.push_back({Token::Type::KW_DOUBLE, word, tokenStartIdx});
            }
            else
            {
                tokens.push_back({Token::Type::KW_INT, word, tokenStartIdx});
            }
        }
        else if (isalpha(lookahead) || input[currentIdx] == '_') // Identifier
        {
            std::string word;
            
            while (currentIdx < input.length() &&
                   (isalpha(input[currentIdx]) || isdigit(input[currentIdx]) || input[currentIdx] == '_'))
            {
                word += input[currentIdx];
                ++currentIdx;
            }

            tokens.push_back({Token::Type::FUNC, word, tokenStartIdx});
        }
        else
        {
            throw std::runtime_error("Unknown character while generating tokens");
        }
    }

    tokens.push_back({Token::Type::eof, "", currentIdx});

    return tokens;
}