#pragma once

#include <iostream>
#include <string>

//! Representation of input
class Token
{
public:
    enum class Type {
        COMMA,

        OPEN_ROUND,
        CLOSE_ROUND,
        
        OPEN_SQUARE,
        CLOSE_SQUARE,

        ARROW,

        ARG, // Starting with #
        FUNC,
        
        KW_INT,
        KW_DOUBLE,

        eof,
    };

    Type type;
    std::string data;
    int startIdx;

    //! Returns string of the data. Used for debugging and error handling.
    std::string print() const;
    //! Uses the print() function and has mainly debugging purposes
    friend std::ostream& operator<<(std::ostream&, const Token&);

};