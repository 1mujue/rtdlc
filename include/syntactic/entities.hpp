#pragma once
#include <vector>
#include <string>
#include "lexer/entities.hpp"
#include "syntactic/forward.hpp"

enum class AST_ENUM {
    COMP_UNIT,
    TASK_DEF,
    TASK_PARAMS,
    IN_PARAM_LIST,
    OUT_PARAM_LIST,
    PARAM_DECL,
    TASK_BODY,
    STATE_DECL,
    NODE,
    COMPOSITE_NODE,
    DECORATOR_NODE,
    LEAF_NODE,
    SEQUENCE_NODE,
    SELECTOR_NODE,
    CHILD_BLOCK,
    RETRY_NODE,
    TIMEOUT_NODE,
    SKILL_CALL,
    TASK_CALL,
    CHECK_STMT,
    WAIT_STMT,
    BIND_SECTIONS,
    IN_BINDINGS,
    OUT_BINDINGS,
    IN_BINDING,
    OUT_BINDING,
    VALUE,
    REF,
    COND_EXP,
    LOR_EXP,
    LAND_EXP,
    EQ_EXP,
    REL_EXP,
    ADD_EXP,
    MUL_EXP,
    UNARY_EXP,
    PRIMARY_EXP,
    LITERAL,
    DURATION,
    TYPE,
    BUILTIN_TYPE
};
const char* toString(AST_ENUM);
std::ostream& operator<<(std::ostream&, AST_ENUM);

class AST: public std::enable_shared_from_this<AST>{
protected:
    AST_ENUM type;
public:
    AST(AST_ENUM type) : type(type) {}
    virtual ~AST() {}
    virtual void print(std::ostream& o);
    virtual void accept(ASTVisitor& av) = 0;
};

class ASTCompUnit: public AST{
public:
    std::vector<ASTTaskDefSPtr> taskDefs;
    ASTCompUnit() : AST(AST_ENUM::COMP_UNIT) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTTaskDef: public AST{
public:
    TokenSPtr deftk, tasktk, idtk, lpartk, rpartk;
    ASTTaskParamsSPtr params = nullptr;
    ASTTaskBodySPtr body;
    ASTTaskDef() : AST(AST_ENUM::TASK_DEF) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTTaskParams: public AST{
public:
    ASTInParamListSPtr inParams = nullptr;
    TokenSPtr semitk;
    ASTOutParamListSPtr outParams = nullptr;
    ASTTaskParams() : AST(AST_ENUM::TASK_PARAMS) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTInParamList: public AST{
public:
    std::vector<ASTParamDeclSPtr> paramDecls;
    std::vector<TokenSPtr> commatks;
    ASTInParamList() : AST(AST_ENUM::IN_PARAM_LIST) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTOutParamList: public AST{
public:
    std::vector<ASTParamDeclSPtr> paramDecls;
    std::vector<TokenSPtr> commatks;
    ASTOutParamList() : AST(AST_ENUM::OUT_PARAM_LIST) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTParamDecl: public AST{
public:
    TokenSPtr idtk, coltk;
    ASTTypeSPtr type;
    ASTParamDecl() : AST(AST_ENUM::PARAM_DECL) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTTaskBody: public AST{
public:
    TokenSPtr lbretk, rbretk;
    std::vector<ASTStateDeclSPtr> stateDecls;
    ASTNodeSPtr node;
    ASTTaskBody() : AST(AST_ENUM::TASK_BODY) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTStateDecl: public AST{
public:
    TokenSPtr stattk, coltk, idtk, semitk;
    ASTTypeSPtr type;
    ASTStateDecl() : AST(AST_ENUM::STATE_DECL) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTNode: public AST{
public:
    ASTComNodeSPtr comNode = nullptr;
    ASTDecNodeSPtr decNode = nullptr;
    ASTLeafNodeSPtr leafNode = nullptr;
    ASTNode() : AST(AST_ENUM::NODE) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTComNode: public AST{
public:
    ASTSeqNodeSPtr seqNode = nullptr;
    ASTSelNodeSPtr selNode = nullptr;
    ASTComNode() : AST(AST_ENUM::COMPOSITE_NODE) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTSeqNode: public AST{
public:
    TokenSPtr seqtk;
    ASTChildBlockSPtr childBlock;
    ASTSeqNode() : AST(AST_ENUM::SEQUENCE_NODE) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTSelNode: public AST{
public:
    TokenSPtr seltk;
    ASTChildBlockSPtr childBlock;
    ASTSelNode() : AST(AST_ENUM::SELECTOR_NODE) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTChildBlock: public AST{
public:
    TokenSPtr lbretk, rbretk;
    std::vector<ASTNodeSPtr> nodes;
    ASTChildBlock() : AST(AST_ENUM::CHILD_BLOCK) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTDecNode: public AST{
public:
    ASTRetryNodeSPtr retryNode = nullptr;
    ASTToutNodeSPtr toutNode = nullptr;
    ASTDecNode() : AST(AST_ENUM::DECORATOR_NODE) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTRetryNode: public AST{
public:
    TokenSPtr retrytk, lpartk, rpartk;
    int val;
    ASTNodeSPtr node;
    ASTRetryNode() : AST(AST_ENUM::RETRY_NODE) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTToutNode: public AST{
public:
    TokenSPtr touttk, lpartk, rpartk;
    ASTDurationSPtr dur;
    ASTNodeSPtr node;
    ASTToutNode() : AST(AST_ENUM::TIMEOUT_NODE) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTLeafNode: public AST{
public:
    ASTSkillCallSPtr skicall = nullptr;
    ASTTaskCallSPtr taskcall = nullptr;
    ASTCheckStmtSPtr cckStmt = nullptr;
    ASTWaitStmtSPtr waitStmt = nullptr;
    ASTLeafNode() : AST(AST_ENUM::LEAF_NODE) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTSkillCall: public AST{
public:
    TokenSPtr dotk, idtk, lpartk, rpartk, semitk;
    ASTBindSecSPtr bindsec = nullptr;
    ASTSkillCall() : AST(AST_ENUM::SKILL_CALL) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTTaskCall: public AST{
public:
    TokenSPtr calltk, idtk, lpartk, rpartk, semitk;
    ASTBindSecSPtr bindsec = nullptr;
    ASTTaskCall() : AST(AST_ENUM::TASK_CALL) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTBindSec: public AST{
public:
    ASTInBindingsSPtr inbinds = nullptr;
    TokenSPtr semitk;
    ASTOutBindingsSPtr outbinds = nullptr;
    ASTBindSec() : AST(AST_ENUM::BIND_SECTIONS) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTInBindings: public AST{
public:
    std::vector<ASTInBindingSPtr> inbs;
    std::vector<TokenSPtr> commatks;
    ASTInBindings() : AST(AST_ENUM::IN_BINDINGS) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTOutBindings: public AST{
public:
    std::vector<ASTOutBindingSPtr> outbs;
    std::vector<TokenSPtr> commatks;
    ASTOutBindings() : AST(AST_ENUM::OUT_BINDINGS) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTInBinding: public AST{
public:
    TokenSPtr idtk, asgtk;
    ASTValueSPtr value;
    ASTInBinding() : AST(AST_ENUM::IN_BINDING) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTOutBinding: public AST{
public:
    TokenSPtr idtk, arrtk;
    ASTRefSPtr ref;
    ASTOutBinding() : AST(AST_ENUM::OUT_BINDING) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTValue: public AST{
public:
    ASTLiteralSPtr literal = nullptr;
    ASTRefSPtr ref = nullptr;
    ASTValue() : AST(AST_ENUM::VALUE) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTCheckStmt: public AST{
public:
    TokenSPtr ccktk, lpartk, rpartk, semitk;
    ASTCondExpSPtr condExp;
    ASTCheckStmt() : AST(AST_ENUM::CHECK_STMT) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTCondExp : public AST{
public:
    ASTLOrExpSPtr lorExp;
    ASTCondExp() : AST(AST_ENUM::COND_EXP) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTLOrExp : public AST{
public:
    std::vector<ASTLAndExpSPtr> landExps;
    std::vector<TokenSPtr> ortks;
    ASTLOrExp() : AST(AST_ENUM::LOR_EXP) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTLAndExp : public AST{
public:
    std::vector<ASTEqExpSPtr> eqExps;
    std::vector<TokenSPtr> andtks;
    ASTLAndExp() : AST(AST_ENUM::LAND_EXP) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTEqExp : public AST{
public:
    std::vector<ASTRelExpSPtr> relExps;
    std::vector<TokenSPtr> optks;
    ASTEqExp() : AST(AST_ENUM::EQ_EXP) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTRelExp : public AST{
public:
    ASTAddExpSPtr addExp1, addExp2 = nullptr;
    TokenSPtr ops = nullptr;
    ASTRelExp() : AST(AST_ENUM::REL_EXP) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTAddExp : public AST{
public:
    std::vector<ASTMulExpSPtr> mulExps;
    std::vector<TokenSPtr> optks;
    ASTAddExp() : AST(AST_ENUM::ADD_EXP) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTMulExp : public AST{
public:
    std::vector<ASTUnaryExpSPtr> unaryExps;
    std::vector<TokenSPtr> optks;
    ASTMulExp() : AST(AST_ENUM::MUL_EXP) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTUnaryExp : public AST{
public:
    TokenSPtr optk = nullptr;
    ASTUnaryExpSPtr unaryExp = nullptr;
    ASTPrimaryExpSPtr primaryExp = nullptr;
    ASTUnaryExp() : AST(AST_ENUM::UNARY_EXP) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTPrimaryExp : public AST{
public:
    ASTLiteralSPtr literal = nullptr;
    ASTRefSPtr ref = nullptr;
    TokenSPtr lpartk = nullptr, rpartk = nullptr;
    ASTCondExpSPtr condExp = nullptr;
    ASTPrimaryExp() : AST(AST_ENUM::PRIMARY_EXP) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTWaitStmt : public AST{
public:
    TokenSPtr waittk, lpartk, rpartk, semitk;
    ASTDurationSPtr dur;
    ASTWaitStmt() : AST(AST_ENUM::WAIT_STMT) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTRef: public AST{
public:
    TokenSPtr idtk;
    ASTRef() : AST(AST_ENUM::REF) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTLiteral : public AST{
public:
    enum class AST_LITERAL_TYPE{
        INT,
        FLOAT,
        STRING,
        BOOL
    };
    AST_LITERAL_TYPE ltype;
    double val;
    std::string content;
    ASTLiteral() : AST(AST_ENUM::LITERAL) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTDuration : public AST{
public:
    int val;
    ASTDuration() : AST(AST_ENUM::DURATION) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTType : public AST{
public:
    ASTBuiltinTypeSPtr buildinType;
    ASTType() : AST(AST_ENUM::TYPE) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};

class ASTBuiltinType : public AST{
public:
    TokenSPtr booltk = nullptr, inttk = nullptr, floattk = nullptr, stringtk = nullptr;
    ASTBuiltinType() : AST(AST_ENUM::BUILTIN_TYPE) {}
    void print(std::ostream& o) override;
    void accept(ASTVisitor& av) override;
};