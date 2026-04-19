#include "syntactic/entities.hpp"
#include "syntactic/ast_visitor.hpp"
#include <string>
const char* toString(AST_ENUM type) {
    switch (type) {
        case AST_ENUM::COMP_UNIT:       return "COMP_UNIT";
        case AST_ENUM::TASK_DEF:        return "TASK_DEF";
        case AST_ENUM::TASK_PARAMS:     return "TASK_PARAMS";
        case AST_ENUM::IN_PARAM_LIST:   return "IN_PARAM_LIST";
        case AST_ENUM::OUT_PARAM_LIST:  return "OUT_PARAM_LIST";
        case AST_ENUM::PARAM_DECL:      return "PARAM_DECL";
        case AST_ENUM::TASK_BODY:       return "TASK_BODY";
        case AST_ENUM::STATE_DECL:      return "STATE_DECL";
        case AST_ENUM::NODE:            return "NODE";
        case AST_ENUM::COMPOSITE_NODE:  return "COMPOSITE_NODE";
        case AST_ENUM::DECORATOR_NODE:  return "DECORATOR_NODE";
        case AST_ENUM::LEAF_NODE:       return "LEAF_NODE";
        case AST_ENUM::SEQUENCE_NODE:   return "SEQUENCE_NODE";
        case AST_ENUM::SELECTOR_NODE:   return "SELECTOR_NODE";
        case AST_ENUM::CHILD_BLOCK:     return "CHILD_BLOCK";
        case AST_ENUM::RETRY_NODE:      return "RETRY_NODE";
        case AST_ENUM::TIMEOUT_NODE:    return "TIMEOUT_NODE";
        case AST_ENUM::SKILL_CALL:      return "SKILL_CALL";
        case AST_ENUM::TASK_CALL:       return "TASK_CALL";
        case AST_ENUM::CHECK_STMT:      return "CHECK_STMT";
        case AST_ENUM::WAIT_STMT:       return "WAIT_STMT";
        case AST_ENUM::BIND_SECTIONS:   return "BIND_SECTIONS";
        case AST_ENUM::IN_BINDINGS:     return "IN_BINDINGS";
        case AST_ENUM::OUT_BINDINGS:    return "OUT_BINDINGS";
        case AST_ENUM::IN_BINDING:      return "IN_BINDING";
        case AST_ENUM::OUT_BINDING:     return "OUT_BINDING";
        case AST_ENUM::VALUE:           return "VALUE";
        case AST_ENUM::REF:             return "REF";
        case AST_ENUM::COND_EXP:        return "COND_EXP";
        case AST_ENUM::LOR_EXP:         return "LOR_EXP";
        case AST_ENUM::LAND_EXP:        return "LAND_EXP";
        case AST_ENUM::EQ_EXP:          return "EQ_EXP";
        case AST_ENUM::REL_EXP:         return "REL_EXP";
        case AST_ENUM::ADD_EXP:         return "ADD_EXP";
        case AST_ENUM::MUL_EXP:         return "MUL_EXP";
        case AST_ENUM::UNARY_EXP:       return "UNARY_EXP";
        case AST_ENUM::PRIMARY_EXP:     return "PRIMARY_EXP";
        case AST_ENUM::LITERAL:         return "LITERAL";
        case AST_ENUM::DURATION:        return "DURATION";
        case AST_ENUM::TYPE:            return "TYPE";
        case AST_ENUM::BUILTIN_TYPE:    return "BUILTIN_TYPE";
        default:                        return "UNKNOWN_AST_ENUM";
    }
}
std::ostream& operator<<(std::ostream& out, AST_ENUM ae){
    return out << toString(ae);
}

static std::string astPrintIdent;
void AST::print(std::ostream& o) {
    o << astPrintIdent;
    o << "AST(Type: " << toString(type) << ")\n";
}

void ASTCompUnit::print(std::ostream& o) {
    astPrintIdent = "";
    AST::print(o);
    for (const auto& it : taskDefs) {
        if (it) it->print(o);
    }
}

void ASTTaskDef::print(std::ostream& o) {
    AST::print(o);
    if (params) params->print(o);
    if (body) body->print(o);
}

void ASTTaskParams::print(std::ostream& o) {
    AST::print(o);
    if (inParams) inParams->print(o);
    if (outParams) outParams->print(o);
}

void ASTInParamList::print(std::ostream& o) {
    AST::print(o);
    for (const auto& it : paramDecls) {
        if (it) it->print(o);
    }
}

void ASTOutParamList::print(std::ostream& o) {
    AST::print(o);
    for (const auto& it : paramDecls) {
        if (it) it->print(o);
    }
}

void ASTParamDecl::print(std::ostream& o) {
    AST::print(o);
    if (type) type->print(o);
}

void ASTTaskBody::print(std::ostream& o) {
    AST::print(o);
    astPrintIdent += "    ";
    for (const auto& it : stateDecls) {
        if (it) {
            it->print(o);
        }
    }
    if (node){
        node->print(o);
    } 
    for(int i = 0; i < 4; i++) astPrintIdent.pop_back();
}

void ASTStateDecl::print(std::ostream& o) {
    AST::print(o);
    if (type) type->print(o);
}

void ASTNode::print(std::ostream& o) {
    AST::print(o);
    if (comNode) comNode->print(o);
    if (decNode) decNode->print(o);
    if (leafNode) leafNode->print(o);
}

void ASTComNode::print(std::ostream& o) {
    AST::print(o);
    if (seqNode) seqNode->print(o);
    if (selNode) selNode->print(o);
}

void ASTSeqNode::print(std::ostream& o) {
    AST::print(o);
    if (childBlock) childBlock->print(o);
}

void ASTSelNode::print(std::ostream& o) {
    AST::print(o);
    if (childBlock) childBlock->print(o);
}

void ASTChildBlock::print(std::ostream& o) {
    AST::print(o);
    astPrintIdent += "    ";
    for (const auto& it : nodes) {
        if (it) {
            it->print(o);
        }
    }
    for(int i = 0; i < 4; i++) astPrintIdent.pop_back();
}

void ASTDecNode::print(std::ostream& o) {
    AST::print(o);
    if (retryNode) retryNode->print(o);
    if (toutNode) toutNode->print(o);
}

void ASTRetryNode::print(std::ostream& o) {
    AST::print(o);
    if (node) node->print(o);
}

void ASTToutNode::print(std::ostream& o) {
    AST::print(o);
    if (dur) dur->print(o);
    if (node) node->print(o);
}

void ASTLeafNode::print(std::ostream& o) {
    AST::print(o);
    if (skicall) skicall->print(o);
    if (taskcall) taskcall->print(o);
    if (cckStmt) cckStmt->print(o);
    if (waitStmt) waitStmt->print(o);
}

void ASTSkillCall::print(std::ostream& o) {
    AST::print(o);
    if (bindsec) bindsec->print(o);
}

void ASTTaskCall::print(std::ostream& o) {
    AST::print(o);
    if (bindsec) bindsec->print(o);
}

void ASTBindSec::print(std::ostream& o) {
    AST::print(o);
    if (inbinds) inbinds->print(o);
    if (outbinds) outbinds->print(o);
}

void ASTInBindings::print(std::ostream& o) {
    AST::print(o);
    for (const auto& it : inbs) {
        if (it) it->print(o);
    }
}

void ASTOutBindings::print(std::ostream& o) {
    AST::print(o);
    for (const auto& it : outbs) {
        if (it) it->print(o);
    }
}

void ASTInBinding::print(std::ostream& o) {
    AST::print(o);
    if (value) value->print(o);
}

void ASTOutBinding::print(std::ostream& o) {
    AST::print(o);
    if (ref) ref->print(o);
}

void ASTValue::print(std::ostream& o) {
    AST::print(o);
    if (literal) literal->print(o);
    if (ref) ref->print(o);
}

void ASTCheckStmt::print(std::ostream& o) {
    AST::print(o);
    if (condExp) condExp->print(o);
}

void ASTCondExp::print(std::ostream& o) {
    AST::print(o);
    if (lorExp) lorExp->print(o);
}

void ASTLOrExp::print(std::ostream& o) {
    AST::print(o);
    for (const auto& it : landExps) {
        if (it) it->print(o);
    }
}

void ASTLAndExp::print(std::ostream& o) {
    AST::print(o);
    for (const auto& it : eqExps) {
        if (it) it->print(o);
    }
}

void ASTEqExp::print(std::ostream& o) {
    AST::print(o);
    for (const auto& it : relExps) {
        if (it) it->print(o);
    }
}

void ASTRelExp::print(std::ostream& o) {
    AST::print(o);
    if (addExp1) addExp1->print(o);
    if (addExp2) addExp2->print(o);
}

void ASTAddExp::print(std::ostream& o) {
    AST::print(o);
    for (const auto& it : mulExps) {
        if (it) it->print(o);
    }
}

void ASTMulExp::print(std::ostream& o) {
    AST::print(o);
    for (const auto& it : unaryExps) {
        if (it) it->print(o);
    }
}

void ASTUnaryExp::print(std::ostream& o) {
    AST::print(o);
    if (unaryExp) unaryExp->print(o);
    if (primaryExp) primaryExp->print(o);
}

void ASTPrimaryExp::print(std::ostream& o) {
    AST::print(o);
    if (literal) literal->print(o);
    if (ref) ref->print(o);
    if (condExp) condExp->print(o);
}

void ASTWaitStmt::print(std::ostream& o) {
    AST::print(o);
    if (dur) dur->print(o);
}

void ASTRef::print(std::ostream& o) {
    AST::print(o);
}

void ASTLiteral::print(std::ostream& o) {
    AST::print(o);
}

void ASTDuration::print(std::ostream& o) {
    AST::print(o);
}

void ASTType::print(std::ostream& o) {
    AST::print(o);
    if (buildinType) buildinType->print(o);
}

void ASTBuiltinType::print(std::ostream& o) {
    AST::print(o);
}

void ASTCompUnit::accept(ASTVisitor& av) {
    av.visitCompUnit(*this);
}

void ASTTaskDef::accept(ASTVisitor& av) {
    av.visitTaskDef(*this);
}

void ASTTaskParams::accept(ASTVisitor& av) {
    av.visitTaskParams(*this);
}

void ASTInParamList::accept(ASTVisitor& av) {
    av.visitInParamList(*this);
}

void ASTOutParamList::accept(ASTVisitor& av) {
    av.visitOutParamList(*this);
}

void ASTParamDecl::accept(ASTVisitor& av) {
    av.visitParamDecl(*this);
}

void ASTTaskBody::accept(ASTVisitor& av) {
    av.visitTaskBody(*this);
}

void ASTStateDecl::accept(ASTVisitor& av) {
    av.visitStateDecl(*this);
}

void ASTNode::accept(ASTVisitor& av) {
    av.visitNode(*this);
}

void ASTComNode::accept(ASTVisitor& av) {
    av.visitComNode(*this);
}

void ASTDecNode::accept(ASTVisitor& av) {
    av.visitDecNode(*this);
}

void ASTLeafNode::accept(ASTVisitor& av) {
    av.visitLeafNode(*this);
}

void ASTSeqNode::accept(ASTVisitor& av) {
    av.visitSeqNode(*this);
}

void ASTSelNode::accept(ASTVisitor& av) {
    av.visitSelNode(*this);
}

void ASTChildBlock::accept(ASTVisitor& av) {
    av.visitChildBlock(*this);
}

void ASTRetryNode::accept(ASTVisitor& av) {
    av.visitRetryNode(*this);
}

void ASTToutNode::accept(ASTVisitor& av) {
    av.visitToutNode(*this);
}

void ASTSkillCall::accept(ASTVisitor& av) {
    av.visitSkillCall(*this);
}

void ASTTaskCall::accept(ASTVisitor& av) {
    av.visitTaskCall(*this);
}

void ASTCheckStmt::accept(ASTVisitor& av) {
    av.visitCheckStmt(*this);
}

void ASTWaitStmt::accept(ASTVisitor& av) {
    av.visitWaitStmt(*this);
}

void ASTBindSec::accept(ASTVisitor& av) {
    av.visitBindSec(*this);
}

void ASTInBindings::accept(ASTVisitor& av) {
    av.visitInBindings(*this);
}

void ASTInBinding::accept(ASTVisitor& av) {
    av.visitInBinding(*this);
}

void ASTOutBindings::accept(ASTVisitor& av) {
    av.visitOutBindings(*this);
}

void ASTOutBinding::accept(ASTVisitor& av) {
    av.visitOutBinding(*this);
}

void ASTValue::accept(ASTVisitor& av) {
    av.visitValue(*this);
}

void ASTCondExp::accept(ASTVisitor& av) {
    av.visitCondExp(*this);
}

void ASTLOrExp::accept(ASTVisitor& av) {
    av.visitLOrExp(*this);
}

void ASTLAndExp::accept(ASTVisitor& av) {
    av.visitLAndExp(*this);
}

void ASTEqExp::accept(ASTVisitor& av) {
    av.visitEqExp(*this);
}

void ASTRelExp::accept(ASTVisitor& av) {
    av.visitRelExp(*this);
}

void ASTAddExp::accept(ASTVisitor& av) {
    av.visitAddExp(*this);
}

void ASTMulExp::accept(ASTVisitor& av) {
    av.visitMulExp(*this);
}

void ASTUnaryExp::accept(ASTVisitor& av) {
    av.visitUnaryExp(*this);
}

void ASTPrimaryExp::accept(ASTVisitor& av) {
    av.visitPrimaryExp(*this);
}

void ASTLiteral::accept(ASTVisitor& av) {
    av.visitLiteral(*this);
}

void ASTRef::accept(ASTVisitor& av) {
    av.visitRef(*this);
}

void ASTDuration::accept(ASTVisitor& av) {
    av.visitDuration(*this);
}

void ASTType::accept(ASTVisitor& av) {
    av.visitType(*this);
}

void ASTBuiltinType::accept(ASTVisitor& av) {
    av.visitBuiltinType(*this);
}