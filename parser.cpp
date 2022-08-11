#include "parser.h"
#include "interpreter.h"



Node::Node(Token token)
    : token(token)
{
    ;
}

void Node::print(std::ostream& out) const
{
    out << token;
}

IntNode::IntNode(Token token)
    : Node(token)
{
    ;
}

std::shared_ptr<Value> IntNode::eval(FunctionScope &fncScp) const
{
    return std::dynamic_pointer_cast<Value>(
        std::make_shared<IntValue>(std::stoi(token.data))
    );
}

DoubleNode::DoubleNode(Token token)
    : Node(token)
{
    ;
}

std::shared_ptr<Value> DoubleNode::eval(FunctionScope &fncScp) const
{
    return std::dynamic_pointer_cast<Value>(
        std::make_shared<RealValue>(std::stod(token.data))
    );
}

ArgumentNode::ArgumentNode(Token token)
    : Node(token)
{
    ;
}

std::shared_ptr<Value> ArgumentNode::eval(FunctionScope &fncScp) const
{
    return fncScp.nth(std::stoi(token.data));
}

ListLiteralNode::ListLiteralNode(Token token, const std::vector<std::shared_ptr<Node>> &contents)
    : Node(token), contents(contents)
{
    ;
}

std::shared_ptr<Value> ListLiteralNode::eval(FunctionScope &fncScp) const
{
    std::vector<std::shared_ptr<Value>> list;

    for (std::shared_ptr<Node> item : contents)
    {
        list.push_back(item->eval(fncScp));
    }

    return std::dynamic_pointer_cast<Value>(
        std::make_shared<ListLiteralValue>(list)
    );
}

void ListLiteralNode::print(std::ostream& out) const
{
    out << "{ListLiteral: " << token;

	for (std::shared_ptr<Node> n : contents)
	{
		n->print(out);
		out << ' ';
	}
	out << '}';
}

std::shared_ptr<Value> FunctionDefinition::eval(FunctionScope &fncScp) const
{
    return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(
        (int)fncScp.getGlobalScope().addFunction(std::make_shared<FunctionDefinition>(*this))
    ));
}

void FunctionDefinition::print(std::ostream& out) const
{
    out << "{FunctionDefinition: " << token << ", ";

	out << "Definition: ";
    definition->print(out);

	out << '}';
}

std::shared_ptr<Value> FunctionApplication::eval(FunctionScope &parentScope) const
{
    FunctionScope localScope(parentScope.getGlobalScope(), std::make_shared<FunctionScope>(parentScope), arguments);
    
    return parentScope.getGlobalScope().callFunction(token.data, localScope);
}

void FunctionApplication::print(std::ostream& out) const
{
    out << "{FunctionApplication: " << token << ", ";

	out << "Arguments: {";
    for (std::shared_ptr<Node> arg : arguments)
    {
        arg->print(out);
        out << ", ";
    }

	out << "}}";
}

std::shared_ptr<Node> Parser::parse(std::ostream& out)
{
    std::shared_ptr<Node> ast = expr(out);

    if (eof())
    {
        return ast;
    }

    throw std::runtime_error("Colud not parse the input!");
}

bool Parser::eof()
{
    return _currentToken->type == Token::Type::eof;
}

Parser::Parser(std::vector<Token>::iterator begin)
    : _currentToken(begin)
{
	;
}

std::shared_ptr<Node> Parser::expr(std::ostream& out)
{
    if (eof())
    {
        throw std::runtime_error("Insufficient input provided.\n");
    }

    // Handling wrong input
    if (_currentToken->type == Token::Type::OPEN_ROUND)
    {
        std::string err = "Unexpected '(' occured, missing identifier before it.\nInfo: ";
        err += _currentToken->print();

        throw std::runtime_error(err);
    }
    if (_currentToken->type == Token::Type::CLOSE_ROUND)
    {
        std::string err = "Unexpected ')' occured.\nInfo: ";
        err += _currentToken->print();

        throw std::runtime_error(err);
    }
    if (_currentToken->type == Token::Type::CLOSE_SQUARE)
    {
        std::string err = "Unexpected ']' occured.\nInfo: ";
        err += _currentToken->print();

        throw std::runtime_error(err);
    }
    if (_currentToken->type == Token::Type::COMMA)
    {
        std::string err = "Unexpected ',' occured.\nInfo: ";
        err += _currentToken->print();

        throw std::runtime_error(err);
    }

    // Handle arguments
    if (_currentToken->type == Token::Type::ARG)
    {
        Token tempToken = *_currentToken;

        ++_currentToken;

        return std::dynamic_pointer_cast<Node>(std::make_shared<ArgumentNode>(tempToken));
    }


    // Parsing Int Literal
    if (_currentToken->type == Token::Type::KW_INT)
    {
        Token tempToken = *_currentToken;

        ++_currentToken;

        return std::dynamic_pointer_cast<Node>(std::make_shared<IntNode>(tempToken));
    }

    // Parsing Double Literal
    if (_currentToken->type == Token::Type::KW_DOUBLE)
    {
        Token tempToken = *_currentToken;

        ++_currentToken;

        return std::dynamic_pointer_cast<Node>(std::make_shared<DoubleNode>(tempToken));
    }

    // Parsing List Literal
    if (_currentToken->type == Token::Type::OPEN_SQUARE)
    {
        Token returnToken = *_currentToken;

        ++_currentToken;

        std::vector<std::shared_ptr<Node>> arguments;

        while(_currentToken->type != Token::Type::eof && _currentToken->type != Token::Type::CLOSE_SQUARE)
        {
            std::shared_ptr<Node> elem = expr(out);

            if (!elem)
            {
                std::string err = "Parsing List Literal error occured at: ";
                err += _currentToken->print();

                throw std::runtime_error(err);
            }

            arguments.push_back(elem);
        }

        if (_currentToken->type == Token::Type::CLOSE_SQUARE)
        {
            ++_currentToken;
            return std::dynamic_pointer_cast<Node>(std::make_shared<ListLiteralNode>(returnToken, arguments));
        }

        std::string err = "Expected ']' but got: ";
        err += returnToken.print();

        throw std::runtime_error(err);
    }

    
    // Handling Token::Type::func

    Token f = *_currentToken; // Function token
    ++_currentToken;
    
    if (eof())
    {
        std::string err = "Insufficient input after identifier: ";
        err += f.print();

        throw std::runtime_error(err);
    }

    // We have function definition definition
    if (_currentToken->type == Token::Type::ARROW)
    {
        ++_currentToken;

        std::shared_ptr<Node> definition = expr(out);

        if (definition == nullptr)
        {
            throw std::runtime_error("Problem while parsing function definition\n");
        }

        return std::dynamic_pointer_cast<Node>(std::make_shared<FunctionDefinition>(f, definition));
    }

    if (_currentToken->type != Token::Type::OPEN_ROUND)
    {
        std::string err = "Expected '(' but instead got: ";
        err += _currentToken->print();

        throw std::runtime_error(err);
    }

    ++_currentToken;

    std::vector<std::shared_ptr<Node>> args;
    bool hasMoreArgs = true;

    while (!eof() && _currentToken->type != Token::Type::CLOSE_ROUND && hasMoreArgs)
    {
        std::shared_ptr<Node> arg = expr(out);

        if (arg.get() == nullptr || _currentToken->type == Token::Type::eof)
        {
            std::string err = "Problem while parsing function call. Info: ";
            err += f.print();

            throw std::runtime_error(err);
        }

        args.push_back(arg);

        hasMoreArgs = _currentToken->type == Token::Type::COMMA;
        if (hasMoreArgs)
        {
            ++_currentToken;
        }
    }

    if (_currentToken->type != Token::Type::CLOSE_ROUND)
    {
        std::string err = "Expected ')' but instead got: ";
        err += _currentToken->print();

        throw std::runtime_error(err);
    }

    ++_currentToken;

    // Successfully parsed function call
    return std::dynamic_pointer_cast<Node>(std::make_shared<FunctionApplication>(f, args));
}