#pragma once
#include "errors/entities.hpp"
#include "semantic/forward.hpp"
// #include <unordered_map>
class FrontendContext{
public:
    // lexer
    // syntactic
    // semantic
    TaskSymSPtr currTask;
    SkillSymSPtr currSkill;
    bool isParseInparas;
    SymbolTableSPtr globTable;
    SymbolTableSPtr currTable;
    std::string typeStr;
    // error
    RTDLErrTable errTable;
private:
    FrontendContext(){}
    ~FrontendContext(){}
    FrontendContext(const FrontendContext&) = delete;
    const FrontendContext& operator=(const FrontendContext&) = delete;
public:
    static FrontendContext& getInstance(){
        static FrontendContext context;
        return context;
    }
};