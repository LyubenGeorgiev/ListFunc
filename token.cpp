#include "token.h"


std::ostream& operator<<(std::ostream& out, const Token &t)
{
    out << "{TokenType: ";
    switch (t._type)
    {
    case Token::Type::comma:
        out << "Comma, ";
        break;
    case Token::Type::open_round:
        out << "OPEN_ROUND, ";
        break;
    case Token::Type::close_round:
        out << "CLOSED_ROUND, ";
        break;
    case Token::Type::open_square:
        out << "OPEN_SQARE, ";
        break;
    case Token::Type::close_square:
        out << "CLOSE_SQARE, ";
        break;
    case Token::Type::arrow:
        out << "ARROW, ";
        break;
    case Token::Type::arg:
        out << "ARG, ";
        break;
    case Token::Type::func:
        out << "FUNC, ";
        break;
    case Token::Type::kw_int:
        out << "INT, ";
        break;
    case Token::Type::kw_double:
        out << "DOUBLE, ";
        break;
    case Token::Type::eof:
        out << "EOF, ";
        break;
    default:
        break;
    }

    out << "TokenData: \"" << t._data << "\", TokenStartIndex: " << t._startIdx << "}";

    return out;
}