#pragma once

#include <iostream>
#include <string>


class Token
{
public:
    enum class Type {
/*
        add,
        sub,
        mul,
        div,
        mod, // Int numbers only
        sqrt,
        
        nand,
        
        eq,
        lt, // Less than, function is le(#0, #1)
        
        len,
        head,
        tail,
        list,
        concat,

        to_int, // Casting to int
        
        kw_if,
*/
        comma,

        open_round,
        close_round,
        
        open_square,
        close_square,

        arrow,

        arg, // Starting with #
        func,
        
        kw_int,
        kw_double,

        eof,
    };

    Type _type;
    std::string _data;
    int _startIdx;

    friend std::ostream& operator<<(std::ostream&, const Token&);

};