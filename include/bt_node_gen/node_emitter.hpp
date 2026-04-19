#pragma once
#include <string>
#include <iostream>
#include <optional>
#include "semantic/entities.hpp"
class NodeEmitter{
private:
    std::string cppTypeOf(const std::string&);
    std::string toReqAccExpr(const std::string&);
    std::optional<std::string> findSuccessSrc(SkillSymSPtr);
    std::string toResponseAccExpr(const std::string&);
    void printWithIndent(const std::string&, std::ostream&, int);
    // common
    void emitHPPHeader(SkillSymSPtr);
    void emitCPPHeader(SkillSymSPtr);
    void emitHPPClass(SkillSymSPtr);
    void emitCPPClass(SkillSymSPtr);
    void emitHPPConstruct(SkillSymSPtr);
    void emitCPPConstruct(SkillSymSPtr);
    void emitHPPPorts(SkillSymSPtr);
    void emitCPPPorts(SkillSymSPtr);
    void emitHPPOnFailure(SkillSymSPtr);
    void emitCPPOnFailure(SkillSymSPtr);
    // action
    void emitHPPSetGoal(SkillSymSPtr);
    void emitCPPSetGoal(SkillSymSPtr);
    void emitHPPOnResultReceived(SkillSymSPtr);
    void emitCPPOnResultReceived(SkillSymSPtr);
    // service
    void emitHPPSetReq(SkillSymSPtr);
    void emitCPPSetReq(SkillSymSPtr);
    void emitHPPOnResponseReceived(SkillSymSPtr);
    void emitCPPOnResponseReceived(SkillSymSPtr);
    // a single wrap.
    void emitHPP(SkillSymSPtr);
    void emitCPP(SkillSymSPtr);

    void emitMainArgParse(std::ostream&);
    void emitMainHeader(std::ostream&);
    void emitFacReg(std::ostream&);
    void emitMainBody(std::ostream&);
    void emitBTRunner();
public:
    void emitNode();
};