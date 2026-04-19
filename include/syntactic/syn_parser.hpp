#pragma once
#include "lexer/entities.hpp"
#include "syntactic/entities.hpp"
#include <vector>
#include <stdexcept>

class SynParser{
private:
    std::vector<TokenSPtr> tks;
    int index;
    TOKEN_ENUM peekToken();
    TokenSPtr getTokenWithExcep(TOKEN_ENUM te);
    TokenSPtr getTokenWithoutExcep(TOKEN_ENUM te);
    
    std::vector<ASTSPtr> astStacks;

    void parseCompUnit();
    void parseTaskDef();
    void parseTaskParams();
    void parseInParamList();
    void parseOutParamList();
    void parseParamDecl();
    void parseTaskBody();
    void parseStateDecl();
    void parseNode();
    void parseCompositeNode();
    void parseDecoratorNode();
    void parseLeafNode();
    void parseSequenceNode();
    void parseSelectorNode();
    void parseChildBlock();
    void parseRetryNode();
    void parseTimeoutNode();
    void parseSkillCall();
    void parseTaskCall();
    void parseCheckStmt();
    void parseWaitStmt();
    void parseBindSections();
    void parseInBindings();
    void parseOutBindings();
    void parseInBinding();
    void parseOutBinding();
    void parseValue();
    void parseCondExp();
    void parseLOrExp();
    void parseLAndExp();
    void parseEqExp();
    void parseRelExp();
    void parseAddExp();
    void parseMulExp();
    void parseUnaryExp();
    void parsePrimaryExp();
    void parseLiteral();
    void parseRef();
    void parseDuration();
    void parseType();
    void parseBuiltinType();
public: 
    ASTCompUnitSPtr parse(const std::vector<TokenSPtr>& tks);
};