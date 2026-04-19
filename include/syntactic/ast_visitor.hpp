#pragma once
#include "syntactic/forward.hpp"

class ASTVisitor{
public:
    virtual ~ASTVisitor() = default;

    virtual void visitCompUnit(ASTCompUnit&) = 0;
    virtual void visitTaskDef(ASTTaskDef&) = 0;
    virtual void visitTaskParams(ASTTaskParams&) = 0;
    virtual void visitInParamList(ASTInParamList&) = 0;
    virtual void visitOutParamList(ASTOutParamList&) = 0;
    virtual void visitParamDecl(ASTParamDecl&) = 0;
    virtual void visitTaskBody(ASTTaskBody&) = 0;
    virtual void visitStateDecl(ASTStateDecl&) = 0;
    virtual void visitNode(ASTNode&) = 0;
    virtual void visitComNode(ASTComNode&) = 0;
    virtual void visitDecNode(ASTDecNode&) = 0;
    virtual void visitLeafNode(ASTLeafNode&) = 0;
    virtual void visitSeqNode(ASTSeqNode&) = 0;
    virtual void visitSelNode(ASTSelNode&) = 0;
    virtual void visitChildBlock(ASTChildBlock&) = 0;
    virtual void visitRetryNode(ASTRetryNode&) = 0;
    virtual void visitToutNode(ASTToutNode&) = 0;
    virtual void visitSkillCall(ASTSkillCall&) = 0;
    virtual void visitTaskCall(ASTTaskCall&) = 0;
    virtual void visitCheckStmt(ASTCheckStmt&) = 0;
    virtual void visitWaitStmt(ASTWaitStmt&) = 0;
    virtual void visitBindSec(ASTBindSec&) = 0;
    virtual void visitInBindings(ASTInBindings&) = 0;
    virtual void visitInBinding(ASTInBinding&) = 0;
    virtual void visitOutBindings(ASTOutBindings&) = 0;
    virtual void visitOutBinding(ASTOutBinding&) = 0;
    virtual void visitValue(ASTValue&) = 0;
    virtual void visitCondExp(ASTCondExp&) = 0;
    virtual void visitLOrExp(ASTLOrExp&) = 0;
    virtual void visitLAndExp(ASTLAndExp&) = 0;
    virtual void visitEqExp(ASTEqExp&) = 0;
    virtual void visitRelExp(ASTRelExp&) = 0;
    virtual void visitAddExp(ASTAddExp&) = 0;
    virtual void visitMulExp(ASTMulExp&) = 0;
    virtual void visitUnaryExp(ASTUnaryExp&) = 0;
    virtual void visitPrimaryExp(ASTPrimaryExp&) = 0;
    virtual void visitLiteral(ASTLiteral&) = 0;
    virtual void visitRef(ASTRef&) = 0;
    virtual void visitDuration(ASTDuration&) = 0;
    virtual void visitType(ASTType&) = 0;
    virtual void visitBuiltinType(ASTBuiltinType&) = 0;
};