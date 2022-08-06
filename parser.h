#pragma once

#include "lexer.h"
#include "linked_list.hpp"
#include "return_value.h"

#include <functional>
#include <memory>


struct FunctionScope;

struct Node
{
    Token _token;

	explicit Node(Token token);
    virtual ~Node() = default;

    virtual std::shared_ptr<Value> eval(FunctionScope &execContext) const = 0;

	virtual void print(std::ostream& out) const;

	virtual std::shared_ptr<Node> clone() const = 0;
};

struct IntNode : public Node
{
	explicit IntNode(Token token);

    std::shared_ptr<Value> eval(FunctionScope &execContext) const override;

    std::shared_ptr<Node> clone() const override;
};

struct DoubleNode : public Node
{
	explicit DoubleNode(Token token);

    std::shared_ptr<Value>eval(FunctionScope &execContext) const override;

    std::shared_ptr<Node> clone() const override;
};

struct ListLiteralNode : public Node
{
	linked_list<std::shared_ptr<Node>> contents;

	ListLiteralNode(Token token, const linked_list<std::shared_ptr<Node>> &contents);
	~ListLiteralNode() = default;

    std::shared_ptr<Value> eval(FunctionScope &execContext) const override;

	void print(std::ostream& out) const override;

    std::shared_ptr<Node> clone() const override;
};

struct ArgumentNode : public Node
{
	explicit ArgumentNode(Token token);

    std::shared_ptr<Value> eval(FunctionScope &execContext) const override;

    std::shared_ptr<Node> clone() const override;
};

struct FunctionDefinition : public Node
{
    const std::shared_ptr<Node> _definition;
    size_t _argc;

    FunctionDefinition(Token token, const std::shared_ptr<Node> definition, size_t argc = 1)
        : Node(token), _definition(definition), _argc(argc) {}
	~FunctionDefinition() = default;

    std::shared_ptr<Value> eval(FunctionScope &execContext) const override;

    void print(std::ostream& out) const override;

    std::shared_ptr<Node> clone() const override;
};

struct FunctionApplication : public Node
{
    const std::vector<std::shared_ptr<Node>> arguments;

    FunctionApplication(Token token, const std::vector<std::shared_ptr<Node>> &arguments)
        : Node(token), arguments(arguments) {}
	~FunctionApplication() = default;

    std::shared_ptr<Value> eval(FunctionScope &execContext) const override;

    void print(std::ostream& out) const override;

    std::shared_ptr<Node> clone() const override;

    size_t getArgc() const noexcept
    {
        return arguments.size();
    }
};

struct DefaultFunctionNode : public Node
{
    const std::function<std::shared_ptr<Value>(FunctionScope&)> func;
    const size_t argc;

    DefaultFunctionNode(const std::string &name,
        const std::function<std::shared_ptr<Value>(FunctionScope&)>& func, size_t argc)
        : Node({Token::Type::func, name, -1}), func(func), argc(argc)
    {
    }

    std::shared_ptr<Value> eval(FunctionScope& execContext) const override
    {
        return func(execContext);
    }

    void print(std::ostream& out) const override
    {
        out << "DefaultFunctionNode\n";
    }

    std::shared_ptr<Node> clone() const override
    {
        return std::dynamic_pointer_cast<Node>(
            std::make_shared<DefaultFunctionNode>(_token._data, func, argc)
        );
    }

    size_t getArgc() const noexcept
    {
        return argc;
    }
};

class Parser
{
public:
    Parser(std::vector<Token>::iterator begin);

    std::shared_ptr<Node> parse(std::ostream& out);

    std::shared_ptr<Node> expr(std::ostream& out);

private:
    std::vector<Token>::iterator _currentToken;

    bool eof();

};