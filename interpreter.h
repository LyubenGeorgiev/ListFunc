#pragma once

#include "return_value.h"

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>


struct Node;
struct FunctionDefinition;
struct FunctionScope;

//! Stores function definitions
struct GlobalScope
{
    //! Checks if function is already defined
    bool isFunctionDefined(const std::string& name, size_t argc);

    //! Calls function
    std::shared_ptr<Value> callFunction(const std::string& name, FunctionScope& fncScp);

    //! True if it's a redefinition, false otherwise
    bool addFunction(std::shared_ptr<FunctionDefinition> definition);

    //! Loads the pre-defined functions
    void loadDefaultLibrary();

private:
    std::unordered_map<std::string, std::unordered_map<size_t, std::shared_ptr<FunctionDefinition>>> definitions;

};

//! Stores needed information for function execution
struct FunctionScope
{
    FunctionScope(GlobalScope &globalExecContext,
                  std::shared_ptr<FunctionScope> parentScope,
                  const std::vector<std::shared_ptr<Node>> &parameters) noexcept
        : globalExecContext(globalExecContext),
          parentScope(parentScope),
          parameters(parameters)
    {
    }
    FunctionScope(GlobalScope &globalExecContext,
                  std::shared_ptr<FunctionScope> parentScope,
                  std::vector<std::shared_ptr<Node>>&& parameters) noexcept
        : globalExecContext(globalExecContext),
          parentScope(parentScope),
          parameters(std::move(parameters))
    {
    }

    //! Evals the nth parameter at runtime
    std::shared_ptr<Value> nth(size_t idx) const;

    //! For lazy evaluation purposes returns head of list
    std::shared_ptr<Value> headOfList() const;
    //! For lazy evaluation purposes returns tail of list
    std::shared_ptr<Value> tailOfList() const;

    //! Gets the parameters count
    size_t paramCount() const noexcept { return parameters.size(); }

    //! Accessor for the global execution context
    GlobalScope& getGlobalScope() noexcept { return globalExecContext; }

    //! Accessor for the global execution context
    const GlobalScope& getGlobalScope() const noexcept { return globalExecContext; }

private:
    GlobalScope& globalExecContext;

    // We store the parentScope so we can eval the parameters lazy
    std::shared_ptr<FunctionScope> parentScope;
    const std::vector<std::shared_ptr<Node>> parameters;

};