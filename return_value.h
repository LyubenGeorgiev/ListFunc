#pragma once

#include <string>
#include <vector>
#include <memory>

//! Abstract class for return values
struct Value
{
    enum class Type
    {
        REAL_NUMBER,
        INT_NUMBER,
        LIST_LITERAL,
        INFINITE_LIST,

    };

    Type type;

    Value(Type type) noexcept : type(type) {}

    //! Gets the string representation of the data inside.
    virtual std::string toString() const noexcept = 0;

};

//! Contains double
struct RealValue : public Value
{
    const double value;

    RealValue(double value) noexcept : Value(Type::REAL_NUMBER), value(value) {}

    //! Overriden method of class Value. Converts double to string.
    std::string toString() const noexcept override
    {
        return std::to_string(value);
    }

};

//! Contains int
struct IntValue : public Value
{
    const int value;

    IntValue(int value) noexcept : Value(Type::INT_NUMBER), value(value) {}

    //! Overriden method of class Value. Converts int to string.
    std::string toString() const noexcept override
    {
        return std::to_string(value);
    }

};

//! Class for lists
struct ListValue : public Value
{
    ListValue(Value::Type type) : Value(type)
    {
        if (type != Value::Type::LIST_LITERAL && type != Value::Type::INFINITE_LIST)
        {
            throw std::runtime_error("A list can be either finite or infinite!");
        }
    }

    //! Gets the string representation of the list.
    virtual std::string toString() const noexcept = 0;

};

//! Contains finite list
struct ListLiteralValue : public ListValue
{
    // Turns out vector is faster than forward_list for heavy list operations
    std::vector<std::shared_ptr<Value>> values;

    ListLiteralValue(const std::vector<std::shared_ptr<Value>> &values) noexcept
        : ListValue(Type::LIST_LITERAL), values(values)
    {
    }

    //! Gets the string representation of the data inside.
    std::string toString() const noexcept override;
    
};

//! Contains infinite list
struct InfiniteListValue : public ListValue
{
    double first;
    double difference;

    InfiniteListValue(double first, double difference) noexcept
        : ListValue(Type::INFINITE_LIST), first(first), difference(difference)
    {
        ;
    }

    //! Gets the string representation of the data inside.
    std::string toString() const noexcept override;

    //! Accessor to the n-th element
    std::shared_ptr<Value> nth(size_t idx) const
    {
        return std::shared_ptr<Value>(new RealValue(first + idx * difference));
    }

};