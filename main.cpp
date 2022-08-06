#include "interpreter.h"
#include "lexer.h"
#include "parser.h"
#include "return_value.h"

#include <fstream>
#include <iostream>
#include <list>


int interactiveMode(GlobalScope &globalExecContext)
{
    std::string line;

    while (true)
    {
        std::cout << "user>>";
        std::getline(std::cin, line);
        if (line == "exit")
        {
            break;
        }
        Lexer l(line);
        std::vector<Token> tokens = l.lex();
        // for (Token t : tokens)
        // {
        //     std::cout << t << '\n';
        // }
        Parser p(tokens.begin());
        std::shared_ptr<Node> res = p.parse(std::cout);
        // res->print(std::cout);
        // std::cout << '\n';
        
        FunctionScope execContext(globalExecContext, nullptr, std::vector<std::shared_ptr<Node>>());

        std::shared_ptr<Value> evalResult = res->eval(execContext);
        if (evalResult)
            std::cout << "result> " << evalResult->toString() << std::endl;
    }

    return 0;
}

int main(int argc, const char** argv)
{
    
    GlobalScope globalExecCtx;
    globalExecCtx.loadDefaultLibrary();
    return interactiveMode(globalExecCtx);
}

// test -> if(#0, add(#1, 1), 0)

// int main(/*int argc, const char** argv*/)
//{
//    try
//    {
//        // std::list<funclang::Token> tokens = funclang::lex("123");
//        // funclang::ParsingContext ctx(std::move(tokens));
//        // funclang::RealLiteralParser p;
//
//        // funclang::Parser::ParseResult parseResult = p.parse(ctx);
//        // ctx.consumeTokenCnt(parseResult.numConsumedTokens);
//
//        // assert(ctx.hasReachedEnd());
//        // assert(parseResult.parsedAST->evaluate()->type ==
//        //       funclang::ValueBase::Type::REAL_NUMBER);
//
//        // std::vector<SelfCleaningPtr<AST>> sub;
//        //// AST* _1 = new IntLiteralAST(1);
//        // AST* _2 = new RealLiteralAST(2);
//        // AST* _3 = new IntLiteralAST(3);
//        // AST* _4 = new RealLiteralAST(4);
//
//        // sub.emplace_back(new IntLiteralAST(0));
//        // sub.emplace_back(new IntLiteralAST(1));
//        // sub.emplace_back(_2);
//        // sub.emplace_back(_3);
//        // sub.emplace_back(_4);
//
//        // FiniteListValue flv(std::move(sub));
//
//        // printf("%s", flv.toString().c_str());
//
//        // ----------------
//
//        // const std::string src =
//        //    "[asd(), 23, \n\n\n\n\n\n\n\n\n\n"
//        //    "[23.0, 111]] 123asdasdasdasd asdasdads";
//        // std::list<Token> tokens = lex(src);
//
//        // ListLiteralParser p;
//        // Parser::ParseResult parseResult =
//        p.parse(ParsingContext(std::move(tokens)));
//
//        // ----------------
//
//        // const std::string source = "#2";
//
//        // ParsingContext parsingCtx(lex(source));
//
//        // Parser::ParseResult parseRes =
//        //    ParameterCitationParser().parse(parsingCtx);
//
//        // parsingCtx.consumeTokenCnt(parseRes.numConsumedTokens);
//        // assert(parsingCtx.hasReachedEnd());
//
//        // GlobalExecutionContext globalExecCtx;
//
//        // std::vector<SelfCleaningPtr<Value>> params;
//        // params.emplace_back(new IntValue(0));
//        // params.emplace_back(new IntValue(2));
//        // params.emplace_back(new IntValue(4));
//        // params.emplace_back(new IntValue(6));
//
//        // ExecutionContext execCtx(globalExecCtx, std::move(params));
//
//        // SelfCleaningPtr<Value> execRes =
//        // parseRes.parsedAST->evaluate(execCtx);
//
//        // printf("%s", execRes->toString().c_str());
//
//        // ------------------
//
//        // const std::string firstSrc = "first -> #0";
//        // const std::string zeroSrc = "zero -> 0";
//
//        // FunctionDefinitionAST first("first", 2, new
//        ParameterCitationAST(1));
//
//        // GlobalExecutionContext globalExecContext;
//        //// bool redefinition = globalExecContext.addFunction(first);
//        //// assert(!redefinition);
//
//        // ExecutionContext execCtx(globalExecContext,
//        //                         std::vector<SelfCleaningPtr<Value>>());
//
//        // printf("Definition evaluation: %s\n",
//        //       first.evaluate(execCtx)->toString().c_str());
//
//        // std::vector<SelfCleaningPtr<Value>> params;
//        // params.emplace_back(new IntValue(123));
//        // params.emplace_back(new IntValue(456));
//
//        // std::vector<SelfCleaningPtr<AST>> paramExpressions;
//        // paramExpressions.emplace_back(new ParameterCitationAST(0));
//        // paramExpressions.emplace_back(new ParameterCitationAST(1));
//
//        // FunctionCallAST call("first", std::move(paramExpressions));
//        //// first(#0, #1)
//
//        // ExecutionContext callCtx(globalExecContext, std::move(params));
//
//        // printf("Call: %s\n", call.evaluate(callCtx)->toString().c_str());
//
//        //-------------------------
//
//        // const std::string src = "[1, 2, #0, four()]";
//
//        // ParsingContext pCtx(lex(src));
//
//        // const FuncLangParser generalParser;
//
//        // const Parser::ParseResult parseResult = generalParser.parse(pCtx);
//        // pCtx.consumeTokenCnt(parseResult.numConsumedTokens);
//
//        // GlobalExecutionContext geCtx;
//        // geCtx.addFunction(
//        //    FunctionDefinitionAST("four", 0, new IntLiteralAST(4)));
//        //// four -> 4
//
//        // std::vector<SelfCleaningPtr<Value>> params;
//
//        // params.emplace_back(new RealValue(3));
//
//        // ExecutionContext execCtx(geCtx, std::move(params));
//
//        // printf("%s",
//        parseResult.parsedAST->evaluate(execCtx)->toString().c_str());
//
//        // -------------------------
//
//        const std::string src = "forth->#3 forth(1, 2, 3, 4) ";
//
//        ParsingContext parCtx(lex(src));
//        FuncLangParser generalParser;
//        Parser::ParseResult parseResult = generalParser.parse(parCtx);
//        assert(!parseResult.parsedAST.isNull());
//        assert(parseResult.numConsumedTokens == 4);
//
//        parCtx.consumeTokenCnt(parseResult.numConsumedTokens);
//
//        SelfCleaningPtr<AST>& funcDef = parseResult.parsedAST;
//
//        GlobalExecutionContext globalExecCtx;
//        ExecutionContext execCtx(globalExecCtx,
//                                 std::vector<SelfCleaningPtr<Value>>());
//
//        funcDef->evaluate(execCtx);
//        // function now should be defined
//
//        parseResult = generalParser.parse(parCtx);
//        assert(!parseResult.parsedAST.isNull());
//        assert(parseResult.numConsumedTokens == 10);
//
//        SelfCleaningPtr<AST>& funcCall = parseResult.parsedAST;
//
//        printf("Called: %s", funcCall->evaluate(execCtx)->toString().c_str());
//    }
//    catch (const funclang::LexError& err)
//    {
//        std::cout << err;
//        return ExitCode::PARSE_FAIL;
//    }
//
//    return ExitCode::SUCCESS;
//}