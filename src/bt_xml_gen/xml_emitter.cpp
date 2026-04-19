#include "bt_xml_gen/xml_emitter.hpp"
#include "bt_xml_gen/entities.hpp"
#include "syntactic/entities.hpp"
void XMLEmitter::visitCompUnit(ASTCompUnit& compUnit) {
    XMLNodeSPtr xmlCP = std::make_shared<XMLNode>("root");
    xmlCP->addAttr("BTCPP_format", valueFormat(std::to_string(4)));
    xmlCP->addAttr("main_tree_to_execute", valueFormat("Main"));
    for(const auto& it: compUnit.taskDefs){
        it->accept(*this);
        xmlCP->addChild(xmlStack.back()); xmlStack.pop_back();
    }
    xmlStack.push_back(xmlCP);
}
void XMLEmitter::visitTaskDef(ASTTaskDef& taskDef){
    XMLNodeSPtr xmlTD = std::make_shared<XMLNode>("BehaviorTree");
    xmlTD->addAttr("ID", valueFormat(taskDef.idtk->getContent()));
    taskDef.body->accept(*this);
    xmlTD->addChild(xmlStack.back()); xmlStack.pop_back();
    xmlStack.push_back(xmlTD);
}
void XMLEmitter::visitTaskParams(ASTTaskParams& taskParam) {
    ;
}
void XMLEmitter::visitInParamList(ASTInParamList& InPara) {

}
void XMLEmitter::visitOutParamList(ASTOutParamList& outPara) {}
void XMLEmitter::visitParamDecl(ASTParamDecl& paraDecl) {
    ;
}
void XMLEmitter::visitTaskBody(ASTTaskBody& taskBody) {
    taskBody.node->accept(*this);
}
void XMLEmitter::visitStateDecl(ASTStateDecl& staDecl) {
    ;
}
void XMLEmitter::visitNode(ASTNode& node) {
    if(node.comNode) node.comNode->accept(*this);
    if(node.decNode) node.decNode->accept(*this);
    if(node.leafNode) node.leafNode->accept(*this);
}
void XMLEmitter::visitComNode(ASTComNode& comNode) {
    if(comNode.selNode) comNode.selNode->accept(*this);
    if(comNode.seqNode) comNode.seqNode->accept(*this);
}
void XMLEmitter::visitDecNode(ASTDecNode& decNode) {
    if(decNode.retryNode) decNode.retryNode->accept(*this);
    if(decNode.toutNode) decNode.toutNode->accept(*this);
}
void XMLEmitter::visitLeafNode(ASTLeafNode& leafNode) {
    if(leafNode.skicall) leafNode.skicall->accept(*this);
    if(leafNode.taskcall) leafNode.taskcall->accept(*this);
    if(leafNode.cckStmt) leafNode.cckStmt->accept(*this);
    if(leafNode.waitStmt) leafNode.waitStmt->accept(*this);
}
void XMLEmitter::visitSeqNode(ASTSeqNode& seqNode) {
    XMLNodeSPtr xmlSeq = std::make_shared<XMLNode>("Sequence");
    seqNode.childBlock->accept(*this);
    xmlSeq->addOChild(*xmlStack.back()); xmlStack.pop_back();
    xmlStack.push_back(xmlSeq);
}
void XMLEmitter::visitSelNode(ASTSelNode& selNode) {
    XMLNodeSPtr xmlSel = std::make_shared<XMLNode>("Fallback");
    selNode.childBlock->accept(*this);
    xmlSel->addOChild(*xmlStack.back()); xmlStack.pop_back();
    xmlStack.push_back(xmlSel);
}
void XMLEmitter::visitChildBlock(ASTChildBlock& cb) {
    XMLNodeSPtr xmlCB = std::make_shared<XMLNode>("");
    for(const auto& it: cb.nodes) {
        it->accept(*this);
        xmlCB->addChild(xmlStack.back()); xmlStack.pop_back();
    }
    xmlStack.push_back(xmlCB);
}
void XMLEmitter::visitRetryNode(ASTRetryNode& retry) {
    XMLNodeSPtr xmlRetry = std::make_shared<XMLNode>("RetryUntilSuccessful");
    int temp = retry.val; xmlRetry->addAttr("num_attempts", valueFormat(std::to_string(temp)));
    retry.node->accept(*this);
    xmlRetry->addChild(xmlStack.back()); xmlStack.pop_back();
    xmlStack.push_back(xmlRetry);
}
void XMLEmitter::visitToutNode(ASTToutNode& toutNode) {
    XMLNodeSPtr xmlTout = std::make_shared<XMLNode>("Timeout");
    int val = toutNode.dur->val;
    xmlTout->addAttr("msec", valueFormat(std::to_string(val)));
    toutNode.node->accept(*this);
    xmlTout->addChild(xmlStack.back()); xmlStack.pop_back();
    xmlStack.push_back(xmlTout);
} 
void XMLEmitter::visitSkillCall(ASTSkillCall& skicall) {
    XMLNodeSPtr xmlSKI = std::make_shared<XMLNode>(skicall.idtk->getContent());
    if(skicall.bindsec){
        skicall.bindsec->accept(*this);
        XMLNodeSPtr xmlBindSec = xmlStack.back(); xmlStack.pop_back();
        xmlSKI->addOAttr(*xmlBindSec);
    }
    xmlStack.push_back(xmlSKI);
}
void XMLEmitter::visitTaskCall(ASTTaskCall& tascall) {
    XMLNodeSPtr xmlTaskCall = std::make_shared<XMLNode>("SubTree");
    xmlTaskCall->addAttr("ID", valueFormat(tascall.idtk->getContent()));
    tascall.bindsec->accept(*this);
    XMLNodeSPtr xmlBS = xmlStack.back(); xmlStack.pop_back();
    xmlTaskCall->addOAttr(*xmlBS);
    xmlStack.push_back(xmlTaskCall);
}   
void XMLEmitter::visitCheckStmt(ASTCheckStmt& cckStmt) {
    XMLNodeSPtr xmlCheckStmt = std::make_shared<XMLNode>("Precondition");
    cckStmt.condExp->accept(*this);
    std::string exp = expString.back(); expString.pop_back();
    std::string exp1; 
    for(const auto& it: exp){
        if(it != '{' && it != '}'){
            exp1.push_back(it);
        }
    }
    xmlCheckStmt->addAttr("if", valueFormat(exp1));
    xmlCheckStmt->addAttr("else", valueFormat("FAILURE"));
    XMLNodeSPtr xmlAlwaysSuc = std::make_shared<XMLNode>("AlwaysSuccess");
    xmlCheckStmt->addChild(xmlAlwaysSuc);
    xmlStack.push_back(xmlCheckStmt);
}
void XMLEmitter::visitWaitStmt(ASTWaitStmt& waitStmt) {
    XMLNodeSPtr xmlWait = std::make_shared<XMLNode>("Wait");
    int val = waitStmt.dur->val;
    xmlWait->addAttr("msec", valueFormat(std::to_string(val)));
    xmlStack.push_back(xmlWait);
}
void XMLEmitter::visitBindSec(ASTBindSec& bindSec) {
    XMLNodeSPtr xmlBS = std::make_shared<XMLNode>("");
    if(bindSec.inbinds){
        bindSec.inbinds->accept(*this);
        XMLNodeSPtr inB = xmlStack.back(); xmlStack.pop_back();
        xmlBS->addOAttr(*inB);
    }
    if(bindSec.outbinds){
        bindSec.outbinds->accept(*this);
        XMLNodeSPtr outB = xmlStack.back(); xmlStack.pop_back();
        xmlBS->addOAttr(*outB);
    }
    xmlStack.push_back(xmlBS);
}
void XMLEmitter::visitInBindings(ASTInBindings& inBs) {
    XMLNodeSPtr xmlINBS = std::make_shared<XMLNode>("");
    for(const auto& it: inBs.inbs){
        it->accept(*this); 
        XMLNodeSPtr inB = xmlStack.back(); xmlStack.pop_back();
        xmlINBS->addOAttr(*inB);
    }
    xmlStack.push_back(xmlINBS);
} 
void XMLEmitter::visitInBinding(ASTInBinding& inB) {
    XMLNodeSPtr xmlINB = std::make_shared<XMLNode>("");
    inB.value->accept(*this);
    XMLNodeSPtr xmlVal = xmlStack.back(); xmlStack.pop_back();
    xmlINB->addAttr(inB.idtk->getContent(), valueFormat(xmlVal->getAttr("value")));
    xmlStack.push_back(xmlINB);
}
void XMLEmitter::visitOutBindings(ASTOutBindings& outBS) {
    XMLNodeSPtr xmlOBS = std::make_shared<XMLNode>("");
    for(const auto& it: outBS.outbs){
        it->accept(*this);
        XMLNodeSPtr xmlOB = xmlStack.back(); xmlStack.pop_back();
        xmlOBS->addOAttr(*xmlOB); 
    }
    xmlStack.push_back(xmlOBS);
}
void XMLEmitter::visitOutBinding(ASTOutBinding& ob) {
    XMLNodeSPtr xmlOB = std::make_shared<XMLNode>("");
    ob.ref->accept(*this);
    XMLNodeSPtr xmlRef = xmlStack.back(); xmlStack.pop_back();
    xmlOB->addAttr(ob.idtk->getContent(), valueFormat(xmlRef->getAttr("name")));
    xmlStack.push_back(xmlOB);
}
void XMLEmitter::visitCondExp(ASTCondExp& condExp) {
    condExp.lorExp->accept(*this);
}
void XMLEmitter::visitLOrExp(ASTLOrExp& lOrExp) {
    std::string exp = "";
    int sz = lOrExp.landExps.size();
    for(int i = 0; i < sz; i++){
        lOrExp.landExps[i]->accept(*this);
        exp += expString.back(); expString.pop_back();
        if(i != sz - 1) exp += " || ";
    }
    expString.push_back(exp);
}
void XMLEmitter::visitLAndExp(ASTLAndExp& lAndExp) {
    std::string exp = "";
    int sz = lAndExp.eqExps.size();
    for(int i = 0; i < sz; i++){
        lAndExp.eqExps[i]->accept(*this);
        exp += expString.back(); expString.pop_back();
        if(i != sz - 1) exp += " &amp;&amp; ";
    }
    expString.push_back(exp);
}
void XMLEmitter::visitEqExp(ASTEqExp& eqExp) {
    std::string exp = "";
    int sz = eqExp.relExps.size();
    for(int i = 0; i < sz; i++){
        eqExp.relExps[i]->accept(*this);
        exp += expString.back(); expString.pop_back();
        if(i != sz - 1) exp +=  " " + eqExp.optks[i]->getContent() + " ";
    }
    expString.push_back(exp);
}
void XMLEmitter::visitRelExp(ASTRelExp& relExp) {
    std::string exp = "";
    relExp.addExp1->accept(*this);
    exp += expString.back(); expString.pop_back();
    if(relExp.addExp2){
        std::string opc = relExp.ops->getContent();
        exp += " " + opc + " ";
        relExp.addExp2->accept(*this);
        exp += expString.back(); expString.pop_back();
    }
    expString.push_back(exp);
}
void XMLEmitter::visitAddExp(ASTAddExp& addExp) {
    std::string exp = "";
    int sz = addExp.mulExps.size();
    for(int i = 0; i < sz; i++){
        addExp.mulExps[i]->accept(*this);
        exp += expString.back(); expString.pop_back();
        if(i != sz - 1) exp += " " + addExp.optks[i]->getContent() + " ";
    }
    expString.push_back(exp);
}
void XMLEmitter::visitMulExp(ASTMulExp& mulExp) {
    std::string exp = "";
    int sz = mulExp.unaryExps.size();
    for(int i = 0; i < sz; i++){
        mulExp.unaryExps[i]->accept(*this);
        exp += expString.back(); expString.pop_back();
        if(i != sz - 1) exp += " " + mulExp.optks[i]->getContent() + " ";
    }
    expString.push_back(exp);
}
void XMLEmitter::visitUnaryExp(ASTUnaryExp& unaryExp) {
    if(unaryExp.primaryExp){
        unaryExp.primaryExp->accept(*this);
    } else if(unaryExp.unaryExp){
        std::string exp = unaryExp.optk->getContent();
        unaryExp.unaryExp->accept(*this);
        exp += expString.back(); expString.pop_back();
        expString.push_back(exp);
    }
}
void XMLEmitter::visitPrimaryExp(ASTPrimaryExp& priExp) {
    std::string exp = "";
    if(priExp.condExp){
        exp += "(";
        priExp.condExp->accept(*this);
        exp += expString.back(); expString.pop_back();
        exp += ")";
    } else if(priExp.ref){
        priExp.ref->accept(*this);
        XMLNodeSPtr xmlRef = xmlStack.back(); xmlStack.pop_back();
        exp += xmlRef->getAttr("name");
    } else if(priExp.literal){
        priExp.literal->accept(*this);
        XMLNodeSPtr xmlLiter = xmlStack.back(); xmlStack.pop_back();
        std::string oriVal = xmlLiter->getAttr("value");
        exp += oriVal;
    }
    expString.push_back(exp);
}
void XMLEmitter::visitLiteral(ASTLiteral& literal) {
    XMLNodeSPtr xmlLiteral = std::make_shared<XMLNode>("");
    switch (literal.ltype)
    {
    case ASTLiteral::AST_LITERAL_TYPE::BOOL:
        if((int)literal.val != 0) xmlLiteral->addAttr("value", "true");
        else xmlLiteral->addAttr("value", "false");
        break;
    case ASTLiteral::AST_LITERAL_TYPE::INT:
        xmlLiteral->addAttr("value", std::to_string((int)literal.val));
        break;
    case ASTLiteral::AST_LITERAL_TYPE::DOUBLE:
        xmlLiteral->addAttr("value", std::to_string(literal.val));
        break;
    case ASTLiteral::AST_LITERAL_TYPE::STRING:
        xmlLiteral->addAttr("value", literal.content);
        break;
    default:
        xmlLiteral->addAttr("value", "ERROR VALUE");
        break;
    }
    xmlStack.push_back(xmlLiteral);
}
void XMLEmitter::visitRef(ASTRef& ref) {
    XMLNodeSPtr xmlRef = std::make_shared<XMLNode>("");
    xmlRef->addAttr("name", "{" + ref.idtk->getContent() + "}");
    xmlStack.push_back(xmlRef);
}
void XMLEmitter::visitValue(ASTValue& value) {
    XMLNodeSPtr xmlValue = std::make_shared<XMLNode>("");
    if(value.literal){
        value.literal->accept(*this);
        XMLNodeSPtr xmlLiter = xmlStack.back(); xmlStack.pop_back();
        xmlValue->addAttr("value", xmlLiter->getAttr("value"));
    } else if(value.ref){
        value.ref->accept(*this);
        XMLNodeSPtr xmlRef = xmlStack.back(); xmlStack.pop_back();
        xmlValue->addAttr("value", xmlRef->getAttr("name"));
    }
    xmlStack.push_back(xmlValue);
}
void XMLEmitter::visitDuration(ASTDuration&) {
    ;
}
void XMLEmitter::visitType(ASTType&) {
    ;
}
void XMLEmitter::visitBuiltinType(ASTBuiltinType&) {
    ;
}

XMLNodeSPtr XMLEmitter::emitXML(ASTCompUnitSPtr compUnit){
    compUnit->accept(*this);
    XMLNodeSPtr root = xmlStack.back(); xmlStack.pop_back();
    return root;
}

std::string XMLEmitter::valueFormat(const std::string& ori){
    std::string handled = "";
    handled.push_back('\"');
    for(const char ch: ori){
        if(ch == '<') handled += "%lt;";
        else if(ch == '>') handled += "%gt;";
        else if(ch == '\"') handled += "$quot;";
        else handled.push_back(ch);
    }
    handled.push_back('\"');
    return handled;
}