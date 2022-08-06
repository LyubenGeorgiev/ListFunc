#include "interpreter.h"
// #include "parser.h"

#include <iostream>
#include <stdexcept>


bool GlobalScope::isFunctionDefined(const std::string& name)
{
    return definitions.find(name) != definitions.end();
}

std::shared_ptr<Value> GlobalScope::callFunction(
	const std::string& name, FunctionScope& ctx)
{
    if (!isFunctionDefined(name))
    {
        throw std::runtime_error("Called function which is not defined");
    }

    if (definitions[name]->_argc != ctx.paramCount())
    {
        throw std::runtime_error("Called function with wrong number of parameters");
    }

    return definitions[name]->_definition->eval(ctx);
}

bool GlobalScope::addFunction(std::shared_ptr<FunctionDefinition> definition)
{
    if (isFunctionDefined(definition->_token._data))
    {
        definitions[definition->_token._data] = definition;

        return false;
    }

	definitions[definition->_token._data] = definition;

	return true;
}

size_t GlobalScope::formalParamCountFor(const std::string& functionName)
{
    if (!isFunctionDefined(functionName))
    {
        throw std::runtime_error("Getting parameter count for function which is not defined");
    }

    return definitions[functionName]->_argc;
}

std::shared_ptr<Value> FunctionScope::nth(size_t idx) const
{
    if (idx >= parameters.size())
    {
        throw std::runtime_error(
            "Referencing formal parameter with index outside range of formal "
            "parameter indexes!");
    }

    return parameters[idx]->eval(*parentScope);
}

std::shared_ptr<Value> eqFunc(FunctionScope& execCtx)
{
    const std::shared_ptr<Value> fst = execCtx.nth(0);
    const std::shared_ptr<Value> snd = execCtx.nth(1);

    if (fst->type == snd->type)
    {
        switch (fst->type)
        {
        case Value::Type::INT_NUMBER:
		{
			int fstVal = std::dynamic_pointer_cast<IntValue>(fst)->value;
			int sndVal = std::dynamic_pointer_cast<IntValue>(snd)->value;
			
			return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(fstVal == sndVal));
		}
        case Value::Type::REAL_NUMBER:
		{
			int fstVal = std::dynamic_pointer_cast<RealValue>(fst)->value;
			int sndVal = std::dynamic_pointer_cast<RealValue>(snd)->value;
			
			return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(fstVal == sndVal));
		}
        case Value::Type::LIST_LITERAL:
			throw std::runtime_error("Equality of finite lists is not yet implemented");
        case Value::Type::INFINITE_LIST:
		{
			const std::shared_ptr<InfiniteListValue> f = std::dynamic_pointer_cast<InfiniteListValue>(fst);
			const std::shared_ptr<InfiniteListValue> s = std::dynamic_pointer_cast<InfiniteListValue>(snd);

			return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(
				f->first == s->first && f->difference == s->difference
			));
		}
            break;
            default:
                throw std::runtime_error("Cannot determine if values are equal!");
        }
    }
    else
    {
        throw std::runtime_error("Cannot determine if values of different types are equal!");
    }
}

std::shared_ptr<Value> leFunc(FunctionScope& execCtx)
{
    const std::shared_ptr<Value> fst = execCtx.nth(0);
    const std::shared_ptr<Value> snd = execCtx.nth(1);

    if (fst->type == snd->type)
    {
        switch (fst->type)
        {
		case Value::Type::INT_NUMBER:
		{
			int fstVal = std::dynamic_pointer_cast<IntValue>(fst)->value;
			int sndVal = std::dynamic_pointer_cast<IntValue>(snd)->value;
			
			return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(fstVal < sndVal));
		}
		case Value::Type::REAL_NUMBER:
		{
			int fstVal = std::dynamic_pointer_cast<RealValue>(fst)->value;
			int sndVal = std::dynamic_pointer_cast<RealValue>(snd)->value;
			
			return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(fstVal < sndVal));
		}
		case Value::Type::LIST_LITERAL:
			throw std::runtime_error("Cannot compare 2 lists");
		default:
			throw std::runtime_error("Cannot determine if values of unknown type!");
        }
    }
    else
    {
        throw std::runtime_error("Cannot compare values of different types!");
    }
}

std::shared_ptr<Value> nandFunc(FunctionScope& execCtx)
{
	std::shared_ptr<Value> vals[2];
	vals[0] = execCtx.nth(0);
	vals[1] = execCtx.nth(1);
	bool res[2];

	for (size_t i = 0; i < 2; ++i)
	{
		switch (vals[i]->type)
		{
		case Value::Type::INT_NUMBER:
		{
			res[i] = std::dynamic_pointer_cast<IntValue>(vals[i])->value;
		}
			break;
		case Value::Type::REAL_NUMBER:
		{
			res[i] = std::dynamic_pointer_cast<RealValue>(vals[i])->value;
		}
			break;
		case Value::Type::LIST_LITERAL:
		{
			res[i] = !std::dynamic_pointer_cast<ListLiteralValue>(vals[i])->values.empty();
		}
			break;
		case Value::Type::INFINITE_LIST:
		{
			res[i] = true;
		}
			break;
		default:
			throw std::runtime_error("Cannot nand unknown types!");
        }
	}

	return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(!res[0] || !res[1]));
}

std::shared_ptr<Value> lengthFunc(FunctionScope& execCtx)
{
    const std::shared_ptr<Value> fst = execCtx.nth(0);

    if (fst->type != Value::Type::LIST_LITERAL)
    {
        if (fst->type == Value::Type::INFINITE_LIST)
        {
            throw std::runtime_error("Cannot determine length() of infinite list!");
        }

		throw std::runtime_error("Typing error: the argument to length() must be a list!");
    }

    return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(
		int(std::dynamic_pointer_cast<ListLiteralValue>(fst)->values.size())
	));
}

std::shared_ptr<Value> headFunc(FunctionScope& execCtx)
{
    const std::shared_ptr<Value> fst = execCtx.nth(0);

    if (fst->type == Value::Type::LIST_LITERAL)
    {
		const std::shared_ptr<ListLiteralValue> lst = std::dynamic_pointer_cast<ListLiteralValue>(fst);

        if (!lst->values.empty())
        {
            return lst->values.front();
        }

        throw std::runtime_error("Cannot get head of empty list!");
        
    }
    else if (fst->type == Value::Type::INFINITE_LIST)
    {
        return std::dynamic_pointer_cast<Value>(std::make_shared<RealValue>(
			std::dynamic_pointer_cast<InfiniteListValue>(fst)->first
		));
    }

	throw std::runtime_error("Typing error: the argument to head() must be a list!");
}

std::shared_ptr<Value> tailFunc(FunctionScope& execCtx)
{
    const std::shared_ptr<Value> fst = execCtx.nth(0);

    if (fst->type == Value::Type::LIST_LITERAL)
    {
		linked_list<std::shared_ptr<Value>> &vals = std::dynamic_pointer_cast<ListLiteralValue>(fst)->values;
        vals.pop_front();

        return std::dynamic_pointer_cast<Value>(std::make_shared<ListLiteralValue>(vals));
    }
    else if (fst->type == Value::Type::INFINITE_LIST)
    {
        const std::shared_ptr<InfiniteListValue> lst = std::dynamic_pointer_cast<InfiniteListValue>(fst);

        return std::dynamic_pointer_cast<Value>(std::make_shared<InfiniteListValue>(
			lst->first + lst->difference, lst->difference
		));
    }

	throw std::runtime_error("Typing error: the argument to tail() must be a list!");
}

std::shared_ptr<Value> concatFunc(FunctionScope& execCtx)
{
    const std::shared_ptr<Value> fst = execCtx.nth(0);
    const std::shared_ptr<Value> snd = execCtx.nth(1);

    if (fst->type != Value::Type::INFINITE_LIST && snd->type != Value::Type::LIST_LITERAL)
    {
        throw std::runtime_error(
            "Typing error: the arguments to concat must be finite lists! "
            "Cannot concat infinite lists for obvious reasons");
    }

    linked_list<std::shared_ptr<Value>> &fstVals = std::dynamic_pointer_cast<ListLiteralValue>(fst)->values;
    linked_list<std::shared_ptr<Value>> &sndVals = std::dynamic_pointer_cast<ListLiteralValue>(snd)->values;

    return std::dynamic_pointer_cast<Value>(std::make_shared<ListLiteralValue>(fstVals += sndVals));
}

std::shared_ptr<Value> ifFunc(FunctionScope& execCtx)
{
    const std::shared_ptr<Value> fst = execCtx.nth(0);
    bool condition;

    if (fst->type == Value::Type::INT_NUMBER)
    {
        condition = std::dynamic_pointer_cast<IntValue>(fst)->value;
    }
    else if (fst->type == Value::Type::REAL_NUMBER)
    {
        condition = std::dynamic_pointer_cast<RealValue>(fst)->value;
    }
    else
    {
        throw std::runtime_error(
            "Typing error: the condition of if must be a number - int or real!");
    }

    if (condition)
    {
        std::cout << "TRUE\n";
        return execCtx.nth(1);
    }
    std::cout << "FALSE\n";
    return execCtx.nth(2);
}

std::shared_ptr<Value> readFunc(FunctionScope&)
{
    double val = 0.0;
    std::cin >> val;
    return std::dynamic_pointer_cast<Value>(std::make_shared<RealValue>(val));
}

std::shared_ptr<Value> writeFunc(FunctionScope& execCtx)
{
    try
    {
        std::cout << execCtx.nth(0)->toString() << std::endl;
        return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(0));
    }
    catch (...)
    {
        return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(1));
    }
}

std::shared_ptr<Value> intFunc(FunctionScope& execCtx)
{
    const std::shared_ptr<Value> fst = execCtx.nth(0);

    if (fst->type != Value::Type::REAL_NUMBER)
    {
        throw std::runtime_error("Typing error: the argument to int() must be a real number!");
    }

    return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(
        int(std::dynamic_pointer_cast<RealValue>(fst)->value)
    ));
}

std::shared_ptr<Value> addFunc(FunctionScope& execCtx)
{
    std::shared_ptr<Value> vals[2] = {execCtx.nth(0), execCtx.nth(1)};
    double res = 0;
    bool isDouble = false;

    for (size_t i = 0; i < 2; ++i)
    {
        if (vals[i]->type == Value::Type::REAL_NUMBER)
        {
            res += std::dynamic_pointer_cast<RealValue>(vals[i])->value;
            isDouble = true;
        }
        else if (vals[i]->type == Value::Type::INT_NUMBER)
        {
            res += std::dynamic_pointer_cast<IntValue>(vals[i])->value;
        }
        else
        {
            throw std::runtime_error(
                "Typing error: the arguments to add() must be numbers - int or real!");
        }
    }

    if (isDouble)
    {
        return std::dynamic_pointer_cast<Value>(std::make_shared<RealValue>(res));
    }

    // for rounding purposes while working with double to int
    res += (0.5 - (res < 0));

    return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>((int)res));
}

std::shared_ptr<Value> subFunc(FunctionScope& execCtx)
{
    std::shared_ptr<Value> vals[2] = {execCtx.nth(0), execCtx.nth(1)};
    double res = 0;
    bool isDouble = false;

    for (size_t i = 0; i < 2; ++i)
    {
        if (vals[i]->type == Value::Type::REAL_NUMBER)
        {
            res += (std::dynamic_pointer_cast<RealValue>(vals[i])->value * (1 - 2 * i));
            isDouble = true;
        }
        else if (vals[i]->type == Value::Type::INT_NUMBER)
        {
            res += (std::dynamic_pointer_cast<IntValue>(vals[i])->value * (1 - 2 * i));
        }
        else
        {
            throw std::runtime_error(
                "Typing error: the arguments to sub() must be numbers - int or real!");
        }
    }

    if (isDouble)
    {
        return std::dynamic_pointer_cast<Value>(std::make_shared<RealValue>(res));
    }

    // for rounding purposes while working with double to int
    res += (0.5 - (res < 0));

    return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>((int)res));
}

std::shared_ptr<Value> mulFunc(FunctionScope& execCtx)
{
    std::shared_ptr<Value> vals[2] = {execCtx.nth(0), execCtx.nth(1)};
    double res = 1.0;
    bool isDouble = false;

    for (size_t i = 0; i < 2; ++i)
    {
        if (vals[i]->type == Value::Type::REAL_NUMBER)
        {
            res *= std::dynamic_pointer_cast<RealValue>(vals[i])->value;
            isDouble = true;
        }
        else if (vals[i]->type == Value::Type::INT_NUMBER)
        {
            res *= std::dynamic_pointer_cast<IntValue>(vals[i])->value;
        }
        else
        {
            throw std::runtime_error(
                "Typing error: the arguments to mul() must be numbers - int or real!");
        }
    }

    if (isDouble)
    {
        return std::dynamic_pointer_cast<Value>(std::make_shared<RealValue>(res));
    }

    // for rounding purposes while working with double to int
    res += (0.5 - (res < 0));

    return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>((int)res));
}

std::shared_ptr<Value> divFunc(FunctionScope& execCtx)
{
    const std::shared_ptr<Value> fst = execCtx.nth(0);
    const std::shared_ptr<Value> snd = execCtx.nth(1);

    if ((fst->type != Value::Type::REAL_NUMBER &&
         fst->type != Value::Type::INT_NUMBER) ||
        (snd->type != Value::Type::REAL_NUMBER &&
         snd->type != Value::Type::INT_NUMBER))
    {
        throw std::runtime_error(
            "Typing error: the arguments to div() must be numbers - int or real!");
    }

    if (fst->type == Value::Type::REAL_NUMBER)
    {
        double fstVal = std::dynamic_pointer_cast<RealValue>(fst)->value;

        if (snd->type == Value::Type::REAL_NUMBER)
        {
            double sndVal = std::dynamic_pointer_cast<RealValue>(snd)->value;
            if (sndVal == 0.0)
            {
                throw std::runtime_error("Division by zero!");
            }
            return std::dynamic_pointer_cast<Value>(std::make_shared<RealValue>(fstVal / sndVal));
        }

        int sndVal = std::dynamic_pointer_cast<IntValue>(snd)->value;
        if (sndVal == 0)
        {
            throw std::runtime_error("Division by zero!");
        }

        return std::dynamic_pointer_cast<Value>(std::make_shared<RealValue>(fstVal / sndVal));
    }

    int fstVal = std::dynamic_pointer_cast<IntValue>(fst)->value;
    if (snd->type == Value::Type::REAL_NUMBER)
    {
        double sndVal = std::dynamic_pointer_cast<RealValue>(snd)->value;
        if (sndVal == 0.0)
        {
            throw std::runtime_error("Division by zero!");
        }

        return std::dynamic_pointer_cast<Value>(std::make_shared<RealValue>(fstVal / sndVal));
    }
    
    int sndVal = std::dynamic_pointer_cast<IntValue>(snd)->value;
    if (sndVal == 0)
    {
        throw std::runtime_error("Division by zero!");
    }

    return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(fstVal / sndVal));
}

std::shared_ptr<Value> modFunc(FunctionScope& execCtx)
{
    const std::shared_ptr<Value> fst = execCtx.nth(0);
    const std::shared_ptr<Value> snd = execCtx.nth(1);

    if (fst->type != Value::Type::INT_NUMBER && snd->type != Value::Type::INT_NUMBER)
    {
        throw std::runtime_error("Typing error: the arguments to mod() must be int values!");
    }

    int fstVal = std::dynamic_pointer_cast<IntValue>(fst)->value;
    int sndVal = std::dynamic_pointer_cast<IntValue>(snd)->value;

    if (sndVal == 0)
    {
        throw std::runtime_error("Modulo division by zero!");
    }

    return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(fstVal / sndVal));
}

std::shared_ptr<Value> listFunc(FunctionScope& execCtx)
{
    std::shared_ptr<Value> vals[2] = {execCtx.nth(0), execCtx.nth(1)};
    double res[2];

    for (size_t i = 0; i < 2; ++i)
    {
        if (vals[i]->type == Value::Type::REAL_NUMBER)
        {
            res[i] = std::dynamic_pointer_cast<RealValue>(vals[i])->value;
        }
        else if (vals[i]->type == Value::Type::INT_NUMBER)
        {
            res[i] = std::dynamic_pointer_cast<IntValue>(vals[i])->value;
        }
        else
        {
            throw std::runtime_error("Typing error: the arguments to list() must be numbers!");
        }
    }

    return std::dynamic_pointer_cast<Value>(std::make_shared<InfiniteListValue>(res[0], res[1]));
}

std::shared_ptr<Value> sqrtFunc(FunctionScope& execCtx)
{
    const std::shared_ptr<Value>& fst = execCtx.nth(0);

    if (fst->type == Value::Type::INT_NUMBER)
    {
        return std::dynamic_pointer_cast<Value>(std::make_shared<RealValue>(
            std::sqrt((double)std::dynamic_pointer_cast<IntValue>(fst)->value)
        ));
    }
    else if (fst->type == Value::Type::REAL_NUMBER)
    {
        return std::dynamic_pointer_cast<Value>(std::make_shared<RealValue>(
            std::sqrt(std::dynamic_pointer_cast<RealValue>(fst)->value)
        ));
    }
    
    throw std::runtime_error("Typing error: the arguments to sqrt() must be a number!");
}

void GlobalScope::loadDefaultLibrary()
{
    Token funcTok = {Token::Type::func, "eq", -1};
    std::shared_ptr<FunctionDefinition> eqDef = std::make_shared<FunctionDefinition>(
        funcTok, 
        std::make_shared<DefaultFunctionNode>("eq", eqFunc, 2), 
        2);
    addFunction(eqDef);

    funcTok._data = "le";
    std::shared_ptr<FunctionDefinition> leDef = std::make_shared<FunctionDefinition>(
        funcTok, 
        std::make_shared<DefaultFunctionNode>("le", leFunc, 2), 
        2);
    addFunction(leDef);

    funcTok._data = "nand";
    std::shared_ptr<FunctionDefinition> nandDef = std::make_shared<FunctionDefinition>(
        funcTok, 
        std::make_shared<DefaultFunctionNode>("nand", nandFunc, 2), 
        2);
    addFunction(nandDef);

    funcTok._data = "length";
    std::shared_ptr<FunctionDefinition> lengthDef = std::make_shared<FunctionDefinition>(
        funcTok, 
        std::make_shared<DefaultFunctionNode>("length", lengthFunc, 1), 
        1);
    addFunction(lengthDef);

    funcTok._data = "head";
    std::shared_ptr<FunctionDefinition> headDef = std::make_shared<FunctionDefinition>(
        funcTok, 
        std::make_shared<DefaultFunctionNode>("head", headFunc, 1), 
        1);
    addFunction(headDef);

    funcTok._data = "tail";
    std::shared_ptr<FunctionDefinition> tailDef = std::make_shared<FunctionDefinition>(
        funcTok, 
        std::make_shared<DefaultFunctionNode>("tail", tailFunc, 1), 
        1);
    addFunction(tailDef);

    funcTok._data = "concat";
    std::shared_ptr<FunctionDefinition> concatDef = std::make_shared<FunctionDefinition>(
        funcTok, 
        std::make_shared<DefaultFunctionNode>("concat", concatFunc, 2), 
        2);
    addFunction(concatDef);

    funcTok._data = "if";
    std::shared_ptr<FunctionDefinition> ifDef = std::make_shared<FunctionDefinition>(
        funcTok, 
        std::make_shared<DefaultFunctionNode>("if", ifFunc, 3), 
        3);
    addFunction(ifDef);

    funcTok._data = "read";
    std::shared_ptr<FunctionDefinition> readDef = std::make_shared<FunctionDefinition>(
        funcTok, 
        std::make_shared<DefaultFunctionNode>("read", readFunc, 0), 
        0);
    addFunction(readDef);

    funcTok._data = "write";
    std::shared_ptr<FunctionDefinition> writeDef = std::make_shared<FunctionDefinition>(
        funcTok, 
        std::make_shared<DefaultFunctionNode>("write", writeFunc, 1), 
        1);
    addFunction(writeDef);

    funcTok._data = "int";
    std::shared_ptr<FunctionDefinition> intDef = std::make_shared<FunctionDefinition>(
        funcTok, 
        std::make_shared<DefaultFunctionNode>("int", intFunc, 1), 
        1);
    addFunction(intDef);

    funcTok._data = "add";
    std::shared_ptr<FunctionDefinition> addDef = std::make_shared<FunctionDefinition>(
        funcTok, 
        std::make_shared<DefaultFunctionNode>("add", addFunc, 2), 
        2);
    addFunction(addDef);

    funcTok._data = "sub";
    std::shared_ptr<FunctionDefinition> subDef = std::make_shared<FunctionDefinition>(
        funcTok, 
        std::make_shared<DefaultFunctionNode>("sub", subFunc, 2), 
        2);
    addFunction(subDef);

    funcTok._data = "mul";
    std::shared_ptr<FunctionDefinition> mulDef = std::make_shared<FunctionDefinition>(
        funcTok, 
        std::make_shared<DefaultFunctionNode>("mul", mulFunc, 2), 
        2);
    addFunction(mulDef);

    funcTok._data = "div";
    std::shared_ptr<FunctionDefinition> divDef = std::make_shared<FunctionDefinition>(
        funcTok, 
        std::make_shared<DefaultFunctionNode>("div", divFunc, 2), 
        2);
    addFunction(divDef);

    funcTok._data = "mod";
    std::shared_ptr<FunctionDefinition> modDef = std::make_shared<FunctionDefinition>(
        funcTok, 
        std::make_shared<DefaultFunctionNode>("mod", modFunc, 2), 
        2);
    addFunction(modDef);

    funcTok._data = "sqrt";
    std::shared_ptr<FunctionDefinition> sqrtDef = std::make_shared<FunctionDefinition>(
        funcTok, 
        std::make_shared<DefaultFunctionNode>("sqrt", sqrtFunc, 1), 
        1);
    addFunction(sqrtDef);

    funcTok._data = "list";
    std::shared_ptr<FunctionDefinition> listDef = std::make_shared<FunctionDefinition>(
        funcTok, 
        std::make_shared<DefaultFunctionNode>("list", listFunc, 2), 
        2);
    addFunction(listDef);

}