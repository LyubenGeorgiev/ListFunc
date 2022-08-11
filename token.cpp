#include "token.h"


std::string Token::print() const
{
    std::string res = "{TokenType: ";

    switch (type)
    {
    case Token::Type::COMMA:
        res += "Comma, ";
        break;
    case Token::Type::OPEN_ROUND:
        res += "OPEN_ROUND, ";
        break;
    case Token::Type::CLOSE_ROUND:
        res += "CLOSED_ROUND, ";
        break;
    case Token::Type::OPEN_SQUARE:
        res += "OPEN_SQARE, ";
        break;
    case Token::Type::CLOSE_SQUARE:
        res += "CLOSE_SQARE, ";
        break;
    case Token::Type::ARROW:
        res += "ARROW, ";
        break;
    case Token::Type::ARG:
        res += "ARG, ";
        break;
    case Token::Type::FUNC:
        res += "FUNC, ";
        break;
    case Token::Type::KW_INT:
        res += "INT, ";
        break;
    case Token::Type::KW_DOUBLE:
        res += "DOUBLE, ";
        break;
    case Token::Type::eof:
        res += "EOF, ";
        break;
    default:
        break;
    }

    res += "TokenData: \"";
    res += data;
    res += "\", TokenStartIndex: ";
    res += std::to_string(startIdx);
    res += "}";

    return res;
}

std::ostream& operator<<(std::ostream& out, const Token &t)
{
    out << t.print();

    return out;
}