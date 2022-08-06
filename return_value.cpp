#include "return_value.h"



std::string ListLiteralValue::toString() const noexcept
{
    if (values.empty())
    {
        return "[]";
    }

    const size_t valuesCnt = values.size();

    std::string res = "[";
    bool first = true;
    for (std::shared_ptr<Value> val : values)
    {
        if (!first)
        {
            res += ", ";
        }
        
        res += val->toString();
        first = false;
    }
    res += ']';
    
    return res;
}