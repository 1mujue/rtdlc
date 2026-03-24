#pragma once
#include <vector>
#include "syntactic/ast_visitor.hpp"
#include "xml_gen/entities.hpp"
class XMLEmitter: public ASTVisitor{
private:
    std::vector<XMLNodeSPtr> xmlStack;
    std::vector<std::string> expString;
public:
    void visitCompUnit(ASTCompUnit&) override;
    void visitTaskDef(ASTTaskDef&) override;
    void visitTaskParams(ASTTaskParams&) override;
    void visitInParamList(ASTInParamList&) override;
    void visitOutParamList(ASTOutParamList&) override;
    void visitParamDecl(ASTParamDecl&) override;
    void visitTaskBody(ASTTaskBody&) override;
    void visitStateDecl(ASTStateDecl&) override;
    void visitNode(ASTNode&) override;
    void visitComNode(ASTComNode&) override;
    void visitDecNode(ASTDecNode&) override;
    void visitLeafNode(ASTLeafNode&) override;
    void visitSeqNode(ASTSeqNode&) override;
    void visitSelNode(ASTSelNode&) override;
    void visitChildBlock(ASTChildBlock&) override;
    void visitRetryNode(ASTRetryNode&) override;
    void visitToutNode(ASTToutNode&) override; 
    void visitSkillCall(ASTSkillCall&) override;
    void visitTaskCall(ASTTaskCall&) override;
    void visitCheckStmt(ASTCheckStmt&) override;
    void visitWaitStmt(ASTWaitStmt&) override;
    void visitBindSec(ASTBindSec&) override;
    void visitInBindings(ASTInBindings&) override; 
    void visitInBinding(ASTInBinding&) override;
    void visitOutBindings(ASTOutBindings&) override;
    void visitOutBinding(ASTOutBinding&) override;
    void visitValue(ASTValue&) override;
    void visitCondExp(ASTCondExp&) override;
    void visitLOrExp(ASTLOrExp&) override;
    void visitLAndExp(ASTLAndExp&) override;
    void visitEqExp(ASTEqExp&) override;
    void visitRelExp(ASTRelExp&) override;
    void visitAddExp(ASTAddExp&) override;
    void visitMulExp(ASTMulExp&) override;
    void visitUnaryExp(ASTUnaryExp&) override;
    void visitPrimaryExp(ASTPrimaryExp&) override;
    void visitLiteral(ASTLiteral&) override;
    void visitRef(ASTRef&) override;
    void visitDuration(ASTDuration&) override;
    void visitType(ASTType&) override;
    void visitBuiltinType(ASTBuiltinType&) override;
    XMLNodeSPtr emitXML(ASTCompUnitSPtr);
private:
    std::string valueFormat(const std::string&);
};