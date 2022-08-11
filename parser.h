#pragma once

#include "lexer.h"
#include "return_value.h"

#include <functional>
#include <memory>
#include <cmath>


struct FunctionScope;

//! Abstract syntax tree structure
struct Node
{
    Token token;

	explicit Node(Token token);

    //! Evaluates the AST.
    virtual std::shared_ptr<Value> eval(FunctionScope &fncScp) const = 0;

    //! For debugging purposes.
	virtual void print(std::ostream& out) const;

    //! Needed for figuring out the number of arguments in function definition.
    virtual size_t getArgc() const = 0;

};

//! Abstract syntax tree with int
struct IntNode : public Node
{
	explicit IntNode(Token token);

    //! Evaluates to Value.
    std::shared_ptr<Value> eval(FunctionScope &fncScp) const override;

    size_t getArgc() const override
    {
        return 0;
    }

};

//! Abstract syntax tree with double
struct DoubleNode : public Node
{
	explicit DoubleNode(Token token);

    //! Evaluates to Value.
    std::shared_ptr<Value>eval(FunctionScope &fncScp) const override;

    size_t getArgc() const override
    {
        return 0;
    }
};

//! Abstract syntax tree with list literal
struct ListLiteralNode : public Node
{
	std::vector<std::shared_ptr<Node>> contents;

	ListLiteralNode(Token token, const std::vector<std::shared_ptr<Node>> &contents);

    //! Evaluates to Value.
    std::shared_ptr<Value> eval(FunctionScope &fncScp) const override;

    //! Prints List.
	void print(std::ostream& out) const override;

    //! The argc is the highest index argument.
    size_t getArgc() const override
    {
        size_t res = 0;
        for (std::shared_ptr<Node> node : contents)
        {
            res = std::max(res, node->getArgc());
        }
        return res;
    }
};

//! Abstract syntax tree with argument index
struct ArgumentNode : public Node
{
	explicit ArgumentNode(Token token);

    //! Evaluates to Value.
    std::shared_ptr<Value> eval(FunctionScope &fncScp) const override;

    //! Argc of arg is index + 1
    size_t getArgc() const override
    {
        return std::stoi(token.data) + 1;
    }
};

//! Abstract syntax tree with function definition
struct FunctionDefinition : public Node
{
    const std::shared_ptr<Node> definition;

    FunctionDefinition(Token token, const std::shared_ptr<Node> definition)
        : Node(token), definition(definition) {}

    //! Evaluates to Value.
    std::shared_ptr<Value> eval(FunctionScope &fncScp) const override;

    //! Prints function definition.
    void print(std::ostream& out) const override;

    //! Gets argc from definition argc.
    size_t getArgc() const override
    {
        return definition->getArgc();
    }
};

//! Abstract syntax tree with function application
struct FunctionApplication : public Node
{
    const std::vector<std::shared_ptr<Node>> arguments;

    FunctionApplication(Token token, const std::vector<std::shared_ptr<Node>> &arguments)
        : Node(token), arguments(arguments) {}
	~FunctionApplication() = default;

    //! Evaluates to Value.
    std::shared_ptr<Value> eval(FunctionScope &parentScp) const override;

    //! Prints function application.
    void print(std::ostream& out) const override;

    //! Calculates number of arguments.
    size_t getArgc() const override
    {
        size_t res = 0;
        for (std::shared_ptr<Node> node : arguments)
        {
            res = std::max(res, node->getArgc());
        }
        return res;
    }
};

//! Abstract syntax tree with default function
struct DefaultFunctionNode : public Node
{
    const std::function<std::shared_ptr<Value>(FunctionScope&)> func;
    const size_t argc;

    DefaultFunctionNode(const std::string &name,
        const std::function<std::shared_ptr<Value>(FunctionScope&)>& func, size_t argc)
        : Node({Token::Type::FUNC, name, -1}), func(func), argc(argc)
    {
    }

    //! Evaluates to Value.
    std::shared_ptr<Value> eval(FunctionScope &fncScp) const override
    {
        return func(fncScp);
    }

    //! Prints the name.
    void print(std::ostream& out) const override
    {
        out << "DefaultFunctionNode " << token.data;;
    }

    //! Returns arguments count.
    size_t getArgc() const override
    {
        return argc;
    }
};

//! Parsing vector of Tokens into Abstract Syntax Tree
class Parser
{
public:
    Parser(std::vector<Token>::iterator begin);

    //! Returns Node from vector of Tokens
    std::shared_ptr<Node> parse(std::ostream& out);

private:
    std::vector<Token>::iterator _currentToken;

    //! Helper function for parse()
    std::shared_ptr<Node> expr(std::ostream& out);
    //! Checks for EOF
    bool eof();

};