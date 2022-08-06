#pragma once

#include "linked_list.hpp"

#include <string>
#include <vector>
#include <memory>

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
    virtual ~Value() = default;

    virtual std::string toString() const noexcept = 0;

};

// A real value
struct RealValue : public Value
{
    const double value;

    RealValue(double value) noexcept : Value(Type::REAL_NUMBER), value(value) {}

    std::string toString() const noexcept override
    {
        return std::to_string(value);
    }

};

// An int value
struct IntValue : public Value
{
    const int value;

    IntValue(int value) noexcept : Value(Type::INT_NUMBER), value(value) {}

    std::string toString() const noexcept override
    {
        return std::to_string(value);
    }

};

struct ListValue : public Value
{
    ListValue(Value::Type type) : Value(type)
    {
        if (type != Value::Type::LIST_LITERAL && type != Value::Type::INFINITE_LIST)
        {
            throw std::runtime_error("A list can be either finite or infinite!");
        }
    }

    // From Value
    virtual std::string toString() const noexcept = 0;

};

// A finite list
struct ListLiteralValue : public ListValue
{
    linked_list<std::shared_ptr<Value>> values;

    ListLiteralValue(const linked_list<std::shared_ptr<Value>> &values) noexcept
        : ListValue(Type::LIST_LITERAL), values(values)
    {
    }

    std::string toString() const noexcept override;

    // Accessor to the n-th element
    // std::shared_ptr<Value> nth(size_t idx) const override
    // {
    //     return values[idx];
    // }
    
};

struct InfiniteListValue : public ListValue
{
    double first;
    double difference;

    InfiniteListValue(double first, double difference) noexcept
        : ListValue(Type::INFINITE_LIST), first(first), difference(difference)
    {
        ;
    }

    std::string toString() const noexcept override
    {
        return "INFINITE LIST(start " + std::to_string(first) +
               " and difference " + std::to_string(difference) + ")";
    }

    // Accessor to the n-th element
    std::shared_ptr<Value> nth(size_t idx) const
    {
        return std::shared_ptr<Value>(new RealValue(first + idx * difference));
    }

};