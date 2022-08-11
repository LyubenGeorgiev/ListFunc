#include "interpreter.h"
#include "parser.h"

#include <iostream>
#include <stdexcept>


bool GlobalScope::isFunctionDefined(const std::string& name, size_t argc)
{
    return definitions.find(name) != definitions.end() &&
           definitions[name].find(argc) != definitions[name].end();
}

std::shared_ptr<Value> GlobalScope::callFunction(const std::string& name, FunctionScope& fncScp)
{
    if (!isFunctionDefined(name, fncScp.paramCount()))
    {
        throw std::runtime_error("Called function which is not defined");
    }

    return definitions[name][fncScp.paramCount()]->definition->eval(fncScp);
}

bool GlobalScope::addFunction(std::shared_ptr<FunctionDefinition> definition)
{
    size_t argc = definition->getArgc();
    bool isDefinded = isFunctionDefined(definition->token.data, argc);

	definitions[definition->token.data][argc] = definition;

	return isDefinded;
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

std::shared_ptr<Value> FunctionScope::headOfList() const
{
    if (parameters.empty())
    {
        throw std::runtime_error("head() with no parameters given");
    }

    std::shared_ptr<ListLiteralNode> l = std::dynamic_pointer_cast<ListLiteralNode>(parameters[0]);
    if (l)
    {
        return l->contents[0]->eval(*parentScope);
    }

    const std::shared_ptr<Value> fst = parameters[0]->eval(*parentScope);

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

std::shared_ptr<Value> FunctionScope::tailOfList() const
{
    if (parameters.empty())
    {
        throw std::runtime_error("tail() with no parameters given");
    }

    std::shared_ptr<ListLiteralNode> l = std::dynamic_pointer_cast<ListLiteralNode>(parameters[0]);
    if (l)
    {
        std::vector<std::shared_ptr<Value>> newVals;
        for (size_t i = 1; i < l->contents.size(); ++i)
        {
            newVals.push_back(l->contents[i]->eval(*parentScope));
        }

        return std::dynamic_pointer_cast<Value>(std::make_shared<ListLiteralValue>(newVals));
    }

    const std::shared_ptr<Value> fst = parameters[0]->eval(*parentScope);

    if (fst->type == Value::Type::LIST_LITERAL)
    {
		std::vector<std::shared_ptr<Value>> &vals = std::dynamic_pointer_cast<ListLiteralValue>(fst)->values;
        std::vector<std::shared_ptr<Value>> newVals;
        for (size_t i = 1; i < vals.size(); ++i)
        {
            newVals.push_back(vals[i]);
        }

        return std::dynamic_pointer_cast<Value>(std::make_shared<ListLiteralValue>(newVals));
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

bool eqDouble(double fst, double snd)
{
    const double EPS = 1.0/(1<<30);

    if (std::abs(fst - snd) < EPS)
    {
        return true;
    }

    return false;
}

bool eqHelper(const std::shared_ptr<Value> fst, const std::shared_ptr<Value> snd)
{
    if (fst->type == Value::Type::LIST_LITERAL && fst->type == snd->type)
    {
        std::vector<std::shared_ptr<Value>> &fstVals = std::dynamic_pointer_cast<ListLiteralValue>(fst)->values;
        std::vector<std::shared_ptr<Value>> &sndVals = std::dynamic_pointer_cast<ListLiteralValue>(snd)->values;

        if (fstVals.size() != sndVals.size())
        {
            return false;
        }

        for (size_t i = 0; i < fstVals.size(); ++i)
        {
            if (!eqHelper(fstVals[i], sndVals[i]))
            {
                return false;
            }
        }

        return true;
    }
    else if (fst->type == Value::Type::INFINITE_LIST && fst->type == snd->type)
    {
        const std::shared_ptr<InfiniteListValue> f = std::dynamic_pointer_cast<InfiniteListValue>(fst);
        const std::shared_ptr<InfiniteListValue> s = std::dynamic_pointer_cast<InfiniteListValue>(snd);

        return eqDouble(f->first, s->first) && eqDouble(f->difference, s->difference);
    }
    else if (fst->type == Value::Type::INT_NUMBER && fst->type == snd->type)
    {
        return (std::dynamic_pointer_cast<IntValue>(fst)->value ==
            std::dynamic_pointer_cast<IntValue>(snd)->value
        );
    }
    else if (fst->type == Value::Type::REAL_NUMBER && fst->type == snd->type)
    {
        return eqDouble(std::dynamic_pointer_cast<RealValue>(fst)->value,
            std::dynamic_pointer_cast<RealValue>(snd)->value
        );
    }
    else if (fst->type == Value::Type::INFINITE_LIST || snd->type == Value::Type::INFINITE_LIST)
    {
        return false;
    }
    else if (fst->type == Value::Type::LIST_LITERAL)
    {
        std::vector<std::shared_ptr<Value>> &fstVals = std::dynamic_pointer_cast<ListLiteralValue>(fst)->values;

        if (fstVals.size() != 1)
        {
            return false;
        }

        return eqHelper(fstVals[0], snd);
    }
    else if (snd->type == Value::Type::LIST_LITERAL)
    {
        std::vector<std::shared_ptr<Value>> &sndVals = std::dynamic_pointer_cast<ListLiteralValue>(snd)->values;
        
        if (sndVals.size() != 1)
        {
            return false;
        }

        return eqHelper(fst, sndVals[0]);
    }
    
    double f, s;
    if (fst->type == Value::Type::REAL_NUMBER && snd->type == Value::Type::INT_NUMBER)
    {
        f = std::dynamic_pointer_cast<RealValue>(fst)->value;
        s = std::dynamic_pointer_cast<IntValue>(snd)->value;

        return eqDouble(f, s);
    }
    else if (fst->type == Value::Type::INT_NUMBER && snd->type == Value::Type::REAL_NUMBER)
    {
        f = std::dynamic_pointer_cast<IntValue>(fst)->value;
        s = std::dynamic_pointer_cast<RealValue>(snd)->value;

        return eqDouble(f, s);
    }

    return false;
}

std::shared_ptr<Value> eqFunc(FunctionScope &fncScp)
{
    
    const std::shared_ptr<Value> fst = fncScp.nth(0);
    const std::shared_ptr<Value> snd = fncScp.nth(1);

    return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(eqHelper(fst, snd)));
}

std::shared_ptr<Value> leFunc(FunctionScope &fncScp)
{
    const std::shared_ptr<Value> fst = fncScp.nth(0);
    const std::shared_ptr<Value> snd = fncScp.nth(1);

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
			double fstVal = std::dynamic_pointer_cast<RealValue>(fst)->value;
			double sndVal = std::dynamic_pointer_cast<RealValue>(snd)->value;
			
			return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(fstVal < sndVal));
		}
		case Value::Type::LIST_LITERAL:
			throw std::runtime_error("Cannot compare 2 lists");
		default:
			throw std::runtime_error("Cannot determine if values of unknown type!");
        }
    }

    throw std::runtime_error("Cannot compare values of different types!");
}

std::shared_ptr<Value> nandFunc(FunctionScope &fncScp)
{
	bool res;

	for (size_t i = 0; i < 2; ++i)
	{
        std::shared_ptr<Value> val = fncScp.nth(i);
		switch (val->type)
		{
		case Value::Type::INT_NUMBER:
		{
			res = std::dynamic_pointer_cast<IntValue>(val)->value;
		}
			break;
		case Value::Type::REAL_NUMBER:
		{
			res = std::dynamic_pointer_cast<RealValue>(val)->value;
		}
			break;
		case Value::Type::LIST_LITERAL:
		{
			res = !std::dynamic_pointer_cast<ListLiteralValue>(val)->values.empty();
		}
			break;
		case Value::Type::INFINITE_LIST:
		{
			res = true;
		}
			break;
		default:
			throw std::runtime_error("Cannot nand() unknown types!");
        }

        if (res == false)
        {
            return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(1));
        }
	}

	return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(0));
}

std::shared_ptr<Value> lengthFunc(FunctionScope &fncScp)
{
    const std::shared_ptr<Value> fst = fncScp.nth(0);

    if (fst->type != Value::Type::LIST_LITERAL)
    {
        if (fst->type == Value::Type::INFINITE_LIST)
        {
            throw std::runtime_error("Cannot determine length() of infinite list!");
        }

		return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(-1));
    }

    return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(
		int(std::dynamic_pointer_cast<ListLiteralValue>(fst)->values.size())
	));
}

std::shared_ptr<Value> headFunc(FunctionScope &fncScp)
{
    return fncScp.headOfList();
}

std::shared_ptr<Value> tailFunc(FunctionScope &fncScp)
{
    return fncScp.tailOfList();
}

std::shared_ptr<Value> concatFunc(FunctionScope &fncScp)
{
    const std::shared_ptr<Value> fst = fncScp.nth(0);
    const std::shared_ptr<Value> snd = fncScp.nth(1);

    if (fst->type != Value::Type::LIST_LITERAL || snd->type != Value::Type::LIST_LITERAL)
    {
        throw std::runtime_error(
            "Typing error: the arguments to concat must be finite lists! "
            "Cannot concat infinite lists for obvious reasons");
    }

    std::vector<std::shared_ptr<Value>> &fstVals = std::dynamic_pointer_cast<ListLiteralValue>(fst)->values;
    std::vector<std::shared_ptr<Value>> &sndVals = std::dynamic_pointer_cast<ListLiteralValue>(snd)->values;

    for (std::shared_ptr<Value> val : sndVals)
    {
        fstVals.push_back(val);
    }

    return std::dynamic_pointer_cast<Value>(std::make_shared<ListLiteralValue>(fstVals));
}

std::shared_ptr<Value> ifFunc(FunctionScope &fncScp)
{
    const std::shared_ptr<Value> fst = fncScp.nth(0);
    bool condition;

    if (fst->type == Value::Type::INT_NUMBER)
    {
        condition = std::dynamic_pointer_cast<IntValue>(fst)->value;
    }
    else if (fst->type == Value::Type::REAL_NUMBER)
    {
        condition = std::dynamic_pointer_cast<RealValue>(fst)->value;
    }
    else if (fst->type == Value::Type::LIST_LITERAL)
    {
        condition = !std::dynamic_pointer_cast<ListLiteralValue>(fst)->values.empty();
    }
    else
    {
        throw std::runtime_error(
            "Typing error: the condition of if must be a number - int, real or list literal!");
    }

    if (condition)
    {
        return fncScp.nth(1);
    }

    return fncScp.nth(2);
}

std::shared_ptr<Value> readFunc(FunctionScope &fncScp)
{
    std::string input;
    std::cout << "> read(): ";
    std::getline(std::cin, input);

    std::string::iterator it = input.begin();
    std::string word;
    bool decimal = false, empty = true;

    if (it != input.end() && (*it == '-' || *it == '+'))
    {
        word += *it;
        ++it;
    }

    while (it != input.end() && isdigit(*it))
    {
        empty = false;
        word += *it;
        ++it;
    }

    if (it != input.end() && *it == '.')
    {
        decimal = true;
        word += *it;
        ++it;
    }

    while (it != input.end() && isdigit(*it))
    {
        word += *it;
        ++it;
    }

    if (empty)
    {
        throw std::runtime_error("Empty or invalid input in read()");
    }

    if (decimal)
    {
        return std::dynamic_pointer_cast<Value>(std::make_shared<RealValue>(
            std::stod(word)
        ));
    }
    
    return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(
        std::stoi(word)
    ));
}

std::shared_ptr<Value> writeFunc(FunctionScope &fncScp)
{
    try
    {
        std::cout << fncScp.nth(0)->toString() << std::endl;
        return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(0));
    }
    catch (...)
    {
        return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(1));
    }
}

std::shared_ptr<Value> intFunc(FunctionScope &fncScp)
{
    const std::shared_ptr<Value> fst = fncScp.nth(0);

    if (fst->type == Value::Type::INT_NUMBER)
    {
        return fst;
    }

    if (fst->type != Value::Type::REAL_NUMBER)
    {
        throw std::runtime_error("Typing error: the argument to int() must be a real number!");
    }

    double res = std::dynamic_pointer_cast<RealValue>(fst)->value;
    return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(trunc(res)));
}

std::shared_ptr<Value> addFunc(FunctionScope &fncScp)
{
    std::shared_ptr<Value> vals[2] = {fncScp.nth(0), fncScp.nth(1)};
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

    return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(trunc(res)));
}

std::shared_ptr<Value> subFunc(FunctionScope &fncScp)
{
    std::shared_ptr<Value> vals[2] = {fncScp.nth(0), fncScp.nth(1)};
    double res = 0;
    bool isDouble = false;

    for (int i = 0; i < 2; ++i)
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

    return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(trunc(res)));
}

std::shared_ptr<Value> mulFunc(FunctionScope &fncScp)
{
    std::shared_ptr<Value> vals[2] = {fncScp.nth(0), fncScp.nth(1)};
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

    return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(trunc(res)));
}

std::shared_ptr<Value> divFunc(FunctionScope &fncScp)
{
    const std::shared_ptr<Value> fst = fncScp.nth(0);
    const std::shared_ptr<Value> snd = fncScp.nth(1);

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

std::shared_ptr<Value> modFunc(FunctionScope &fncScp)
{
    const std::shared_ptr<Value> fst = fncScp.nth(0);
    const std::shared_ptr<Value> snd = fncScp.nth(1);

    if (fst->type != Value::Type::INT_NUMBER || snd->type != Value::Type::INT_NUMBER)
    {
        throw std::runtime_error("Typing error: the arguments to mod() must be int values!");
    }

    int fstVal = std::dynamic_pointer_cast<IntValue>(fst)->value;
    int sndVal = std::dynamic_pointer_cast<IntValue>(snd)->value;

    if (sndVal == 0)
    {
        throw std::runtime_error("Modulo division by zero!");
    }

    return std::dynamic_pointer_cast<Value>(std::make_shared<IntValue>(fstVal % sndVal));
}

std::shared_ptr<Value> list1Func(FunctionScope &fncScp)
{
    std::shared_ptr<Value> val = fncScp.nth(0);

    if (val->type == Value::Type::REAL_NUMBER)
    {
        return std::dynamic_pointer_cast<Value>(std::make_shared<InfiniteListValue>(
            std::dynamic_pointer_cast<RealValue>(val)->value, 1
        ));
    }
    else if (val->type == Value::Type::INT_NUMBER)
    {
        return std::dynamic_pointer_cast<Value>(std::make_shared<InfiniteListValue>(
            std::dynamic_pointer_cast<IntValue>(val)->value, 1
        ));
    }

    throw std::runtime_error("Typing error: the arguments to list() must be numbers!");
}

std::shared_ptr<Value> list2Func(FunctionScope &fncScp)
{
    std::shared_ptr<Value> vals[2] = {fncScp.nth(0), fncScp.nth(1)};
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

std::shared_ptr<Value> list3Func(FunctionScope &fncScp)
{
    std::shared_ptr<Value> vals[3] = {fncScp.nth(0), fncScp.nth(1), fncScp.nth(2)};
    bool isDouble = false;;
    double res[2];
    int size;

    for (size_t i = 0; i < 2; ++i)
    {
        if (vals[i]->type == Value::Type::REAL_NUMBER)
        {
            isDouble = true;
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

    if (vals[2]->type != Value::Type::INT_NUMBER)
    {
        throw std::runtime_error("Typing error: #2 for list() should be int!");
    }
    size = std::dynamic_pointer_cast<IntValue>(vals[2])->value;

    std::vector<std::shared_ptr<Value>> values;
    for (int i = 0; i < size; ++i)
    {
        double val = res[0] + res[1] * i;
        if (isDouble)
        {
            values.push_back(std::dynamic_pointer_cast<Value>(
                std::make_shared<RealValue>(val)
            ));
        }
        else
        {
            values.push_back(std::dynamic_pointer_cast<Value>(
                std::make_shared<IntValue>(trunc(val))
            ));
        }
    }

    return std::dynamic_pointer_cast<Value>(std::make_shared<ListLiteralValue>(values));
}

std::shared_ptr<Value> sqrtFunc(FunctionScope &fncScp)
{
    const std::shared_ptr<Value>& fst = fncScp.nth(0);

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
    const std::function<std::shared_ptr<Value>(FunctionScope&)> functions[] = {
        eqFunc, leFunc, nandFunc, lengthFunc, headFunc, tailFunc, concatFunc,
        ifFunc, readFunc, writeFunc, intFunc, addFunc, subFunc, mulFunc, divFunc,
        modFunc, sqrtFunc, list1Func, list2Func, list3Func
    };
    const std::string names[] = {
        "eq", "le", "nand", "length", "head", "tail", "concat",
        "if", "read", "write", "int", "add", "sub", "mul", "div",
        "mod", "sqrt", "list", "list", "list"
    };
    const size_t arguments[] = {
        2, 2, 2, 1, 1, 1, 2, 
        3, 0, 1, 1, 2, 2, 2, 2,
        2, 1, 1, 2, 3
    };
    for (size_t i = 0; i < 20; ++i)
    {
        Token tok = {Token::Type::FUNC, names[i], -1};
        std::shared_ptr<FunctionDefinition> fDef = std::make_shared<FunctionDefinition>(
            tok, 
            std::make_shared<DefaultFunctionNode>(names[i], functions[i], arguments[i]));
        addFunction(fDef);
    }
}