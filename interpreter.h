#pragma once

#include "linked_list.hpp"
#include "return_value.h"
#include "parser.h"

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>


struct GlobalScope
{
    // Checks if a function is defined
    bool isFunctionDefined(const std::string& name);

    // calls the specified function name in the provided execution context
    std::shared_ptr<Value> callFunction(const std::string& name, FunctionScope& ctx);

    // True if it's a redefinition, false otherwise
    bool addFunction(std::shared_ptr<FunctionDefinition> definition);

    // gets the formal parameter count for the function
    size_t formalParamCountFor(const std::string& functionName);

    void loadDefaultLibrary();

private:
    std::unordered_map<std::string, std::shared_ptr<FunctionDefinition>> definitions;
};

// The current execution context for the function
// it can change if we recurse
struct FunctionScope
{
    // Parameters moved in order to preserve on memory
    FunctionScope(GlobalScope &globalExecContext,
                  std::shared_ptr<FunctionScope> parentScope,
                  const std::vector<std::shared_ptr<Node>> &parameters) noexcept
        : globalExecContext(globalExecContext),
          parameters(parameters)
    {
    }
    FunctionScope(GlobalScope &globalExecContext,
                  std::shared_ptr<FunctionScope> parentScope,
                  std::vector<std::shared_ptr<Node>>&& parameters) noexcept
        : globalExecContext(globalExecContext),
          parameters(std::move(parameters))
    {
    }

    // Gets the n-th parameter in the execution context
    // pretty much a parameter citation functionality,
    // but the parameters are always kept inside the execution context
    std::shared_ptr<Value> nth(size_t idx) const;

    // Gets the param count
    size_t paramCount() const noexcept { return parameters.size(); }

    // Accessor for the global execution context
    GlobalScope& getGlobalScope() noexcept
    {
        return globalExecContext;
    }

    // Accessor for the global execution context
    const GlobalScope& getGlobalScope() const noexcept
    {
        return globalExecContext;
    }

private:
    GlobalScope& globalExecContext;

    // This is the root cause of eager evaluation
    // If we change the Value to keep an AST then we can
    // do some lazy evaluation
    // const std::vector<std::shared_ptr<Value>> parameters;
    std::shared_ptr<FunctionScope> parentScope;
    const std::vector<std::shared_ptr<Node>> parameters;

};