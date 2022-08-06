#include "parser.h"
#include "interpreter.h"



Node::Node(Token token)
    : _token(token)
{
    ;
}

void Node::print(std::ostream& out) const
{
    out << _token;
}

IntNode::IntNode(Token token)
    : Node(token)
{
    ;
}

std::shared_ptr<Value> IntNode::eval(FunctionScope &execContext) const
{
    return std::dynamic_pointer_cast<Value>(
        std::make_shared<IntValue>(std::stoi(_token._data))
    );
}

std::shared_ptr<Node> IntNode::clone() const
{
    return std::dynamic_pointer_cast<Node>(std::make_shared<IntNode>(_token));
}

DoubleNode::DoubleNode(Token token)
    : Node(token)
{
    ;
}

std::shared_ptr<Value> DoubleNode::eval(FunctionScope &execContext) const
{
    return std::dynamic_pointer_cast<Value>(
        std::make_shared<RealValue>(std::stod(_token._data))
    );
}

std::shared_ptr<Node> DoubleNode::clone() const
{
    return std::dynamic_pointer_cast<Node>(std::make_shared<DoubleNode>(_token));
}

ArgumentNode::ArgumentNode(Token token)
    : Node(token)
{
    ;
}

std::shared_ptr<Value> ArgumentNode::eval(FunctionScope &execContext) const
{
    return execContext.nth(std::stoi(_token._data));
}

std::shared_ptr<Node> ArgumentNode::clone() const
{
    return std::dynamic_pointer_cast<Node>(std::make_shared<ArgumentNode>(_token));
}

ListLiteralNode::ListLiteralNode(Token token, const linked_list<std::shared_ptr<Node>> &contents)
    : Node(token), contents(contents)
{
    ;
}

std::shared_ptr<Value> ListLiteralNode::eval(FunctionScope &execContext) const
{
    linked_list<std::shared_ptr<Value>> list;

    for (std::shared_ptr<Node> item : contents)
    {
        list.push_back(item->eval(execContext));
    }

    return std::dynamic_pointer_cast<Value>(
        std::make_shared<ListLiteralValue>(list)
    );
}

void ListLiteralNode::print(std::ostream& out) const
{
    out << "{ListLiteral: " << _token;

	for (std::shared_ptr<Node> n : contents)
	{
		n->print(out);
		out << ' ';
	}
	out << '}';
}

std::shared_ptr<Node> ListLiteralNode::clone() const
{
    linked_list<std::shared_ptr<Node>> list;
    for (std::shared_ptr<Node> item : contents)
    {
        list.push_back(item->clone());
    }

    return std::dynamic_pointer_cast<Node>(
        std::make_shared<ListLiteralNode>(_token, list)
    );
}

std::shared_ptr<Value> FunctionDefinition::eval(FunctionScope &execContext) const
{
    return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(
        (int)execContext.getGlobalScope().addFunction(std::dynamic_pointer_cast<FunctionDefinition>(clone()))
    ));
}

void FunctionDefinition::print(std::ostream& out) const
{
    out << "{FunctionDefinition: " << _token << ", ";

	out << "Definition: ";
    _definition->print(out);

	out << '}';
}

std::shared_ptr<Node> FunctionDefinition::clone() const
{
    return std::dynamic_pointer_cast<Node>(
        std::make_shared<FunctionDefinition>(_token, _definition->clone(), _argc)
    );
}

std::shared_ptr<Value> FunctionApplication::eval(FunctionScope &execContext) const
{
    // std::vector<std::shared_ptr<Value>> args;

    // for (std::shared_ptr<Node> arg : arguments)
    // {
    //     args.push_back(arg->eval(execContext));
    // }

    FunctionScope localScope(execContext.getGlobalScope(), std::make_shared<FunctionScope>(execContext), arguments);


    // if (localExecContext.paramCount() <
    //    execContext.getGlobalExecutionContext().formalParamCountFor(_token._data))
    // {
    //    assert(false && "Exception");
    //    return nullptr;
    // }

    return execContext.getGlobalScope().callFunction(_token._data, localScope);
}

void FunctionApplication::print(std::ostream& out) const
{
    out << "{FunctionApplication: " << _token << ", ";

	out << "Arguments: {";
    for (std::shared_ptr<Node> arg : arguments)
    {
        arg->print(out);
        out << ", ";
    }

	out << "}}";
}

std::shared_ptr<Node> FunctionApplication::clone() const
{
    std::vector<std::shared_ptr<Node>> args;
    for (std::shared_ptr<Node> arg : arguments)
    {
        args.push_back(arg->clone());
    }

    return std::dynamic_pointer_cast<Node>(
        std::make_shared<FunctionApplication>(_token, args)
    );
}

std::shared_ptr<Node> Parser::parse(std::ostream& out)
{
    std::shared_ptr<Node> ast = expr(out);

    if (eof())
    {
        return ast;
    }

    std::cout << "Colud not parse the input!\n";
    return nullptr;
}

bool Parser::eof()
{
    return _currentToken->_type == Token::Type::eof;
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
        out << "Insufficient input provided.\n";

        return nullptr;
    }

    // Handling wrong input
    if (_currentToken->_type == Token::Type::open_round)
    {
        out << "Unexpcted '(' occured, missing identifier before it.\nInfo: " << *_currentToken << '\n';

        return nullptr;
    }
    if (_currentToken->_type == Token::Type::close_round)
    {
        out << "Unexpcted ')' occured.\nInfo: " << *_currentToken << '\n';

        return nullptr;
    }
    if (_currentToken->_type == Token::Type::close_square)
    {
        out << "Unexpcted ']' occured.\nInfo: " << *_currentToken << '\n';

        return nullptr;
    }
    if (_currentToken->_type == Token::Type::comma)
    {
        out << "Unexpcted ',' occured.\nInfo: " << *_currentToken << '\n';

        return nullptr;
    }

    // Handle arguments
    if (_currentToken->_type == Token::Type::arg)
    {
        Token tempToken = *_currentToken;

        ++_currentToken;

        return std::dynamic_pointer_cast<Node>(std::make_shared<ArgumentNode>(tempToken));
    }


    // Parsing Int Literal
    if (_currentToken->_type == Token::Type::kw_int)
    {
        Token tempToken = *_currentToken;

        ++_currentToken;

        return std::dynamic_pointer_cast<Node>(std::make_shared<IntNode>(tempToken));
    }

    // Parsing Double Literal
    if (_currentToken->_type == Token::Type::kw_double)
    {
        Token tempToken = *_currentToken;

        ++_currentToken;

        return std::dynamic_pointer_cast<Node>(std::make_shared<DoubleNode>(tempToken));
    }

    // Parsing List Literal
    if (_currentToken->_type == Token::Type::open_square)
    {
        Token returnToken = *_currentToken;

        ++_currentToken;

        linked_list<std::shared_ptr<Node>> arguments;

        while(_currentToken->_type != Token::Type::eof && _currentToken->_type != Token::Type::close_square)
        {
            std::shared_ptr<Node> elem = expr(out);

            if (!elem)
            {
                std::cout << "Parsing List Literal error occured at: " << returnToken << '\n';

                return nullptr;
            }

            arguments.push_back(elem);
        }

        if (_currentToken->_type == Token::Type::close_square)
        {
            ++_currentToken;
            return std::dynamic_pointer_cast<Node>(std::make_shared<ListLiteralNode>(returnToken, arguments));
        }

        std::cout << "Expected ']' but got: " << returnToken << '\n';

        return nullptr;
    }

    
    // Handling Token::Type::func

    Token f = *_currentToken; // Function token
    ++_currentToken;
    
    if (eof())
    {
        out << "Insufficient input after identifier: " << f;

        return nullptr;
    }

    // We have function definition definition
    if (_currentToken->_type == Token::Type::arrow)
    {
        ++_currentToken;

        std::shared_ptr<Node> definition = expr(out);

        if (definition == nullptr)
        {
            out << "Problem while parsing function definition\n";

            return nullptr;
        }

        return std::dynamic_pointer_cast<Node>(std::make_shared<FunctionDefinition>(f, definition));
    }

    if (_currentToken->_type != Token::Type::open_round)
    {
        out << "Expected '(' but instead got: " << *_currentToken << '\n';

        return nullptr;
    }

    ++_currentToken;

    std::vector<std::shared_ptr<Node>> args;
    bool hasMoreArgs = true;

    while (!eof() && _currentToken->_type != Token::Type::close_round && hasMoreArgs)
    {
        std::shared_ptr<Node> arg = expr(out);

        if (arg.get() == nullptr || _currentToken->_type == Token::Type::eof)
        {
            out << "Problem while parsing function call. Info: " << f << '\n';

            return nullptr;
        }

        args.push_back(arg);

        hasMoreArgs = _currentToken->_type == Token::Type::comma;
        if (hasMoreArgs)
        {
            ++_currentToken;
        }
    }

    if (_currentToken->_type != Token::Type::close_round)
    {
        out << "Expected ')' but instead got: " << *_currentToken << '\n';

        return nullptr;
    }

    ++_currentToken;

    // Successfully parsed function call
    return std::dynamic_pointer_cast<Node>(std::make_shared<FunctionApplication>(f, args));

    std::cout << "Parsing function call failed\n";
    return nullptr;
}