#include "semantic/sem_parser.hpp"
#include "semantic/entities.hpp"
#include "syntactic/entities.hpp"
#include "contexts/frontend_context.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
extern FrontendContext& fc;

using json = nlohmann::json;

static void parseParamArray(
    const json& arr,
    Callable& callable,
    bool isInput
) {
    if (!arr.is_array()) {
        throw std::runtime_error("param field must be an array");
    }

    for (const auto& item : arr) {
        if (!item.is_object()) {
            throw std::runtime_error("param entry must be an object");
        }

        std::string name = item.at("name").get<std::string>();
        std::string type = item.at("type").get<std::string>();

        if (isInput) {
            callable.bindInPara(name, type);
        } else {
            callable.bindOutPara(name, type);
        }
    }
}
static void parsePortMappings(const json& j, SkillSym& skill) {
    if (!j.contains("port_mappings")) {
        return;
    }

    const auto& mappings = j.at("port_mappings");

    if (mappings.contains("input_to_ros")) {
        for (const auto& item : mappings.at("input_to_ros")) {
            std::string port   = item.at("port").get<std::string>();
            std::string target = item.at("target").get<std::string>();
            skill.addInputToROSMapping(port, target);
        }
    }

    if (mappings.contains("output_to_ros")) {
        for (const auto& item : mappings.at("output_to_ros")) {
            std::string source = item.at("source").get<std::string>();
            std::string port   = item.at("port").get<std::string>();
            skill.addOutputToROSMapping(source, port);
        }
    }
}
static std::shared_ptr<SkillSym> parseSkillSym(const json& j) {
    std::string skill_name = j.at("name").get<std::string>();
    std::string ros2_backend = j.at("ros2").at("backend").get<std::string>();
    std::string ros2_req_type = j.at("ros2").at("request_type").get<std::string>();
    std::string ros2_srv_name = j.at("ros2").at("service_name").get<std::string>();
    auto skill = std::make_shared<SkillSym>(skill_name);
    skill->setROS2Backend(ros2_backend);
    skill->setROS2ReqType(ros2_req_type);
    skill->setROS2Srv(ros2_srv_name);
    if (j.contains("inputs")) {
        parseParamArray(j.at("inputs"), *skill, true);
    }
    if (j.contains("outputs")) {
        parseParamArray(j.at("outputs"), *skill, false);
    }
    parsePortMappings(j, *skill);

    return skill;
}
void loadSkills(const std::string& path) {
    std::ifstream ifs(path);
    if (!ifs.is_open()) {
        throw std::runtime_error("cannot open skills.json: " + path);
    }

    json root;
    ifs >> root;

    if (!root.is_array()) {
        throw std::runtime_error("skills.json root must be an array");
    }

    for (const auto& item : root) {
        SkillSymSPtr skiSym = parseSkillSym(item);
        fc.globTable->addSymbol(skiSym->getName(), skiSym);
    }
}

void SemParser::parse(ASTCompUnitSPtr compUnit){
    fc.globTable = std::make_shared<SymbolTable>();
    loadSkills("./skills.json");
    fc.currTable = fc.globTable;
    compUnit->accept(*this);
}

void SemParser::visitCompUnit(ASTCompUnit& compUnit){
    for(const auto& it: compUnit.taskDefs){
        it->accept(*this);
    }
}
void SemParser::visitTaskDef(ASTTaskDef& taskDef){
    const std::string& taskName = taskDef.idtk->getContent();
    int line = taskDef.idtk->getLine();
    if(fc.globTable->isSymExistHere(taskName)){
        fc.errTable.addError(RTDLErr{"Semantic Error", line, "The task: " + taskName + " already exists."});
    } else {
        TaskSymSPtr taskSym = std::make_shared<TaskSym>(taskName);
        fc.globTable->addSymbol(taskName, taskSym);
        fc.currTask = taskSym;
    }
    SymbolTableSPtr oriTable = fc.currTable;
    fc.currTable = std::make_shared<SymbolTable>();
    fc.currTable->setFaTable(oriTable);
    oriTable->addChildTable(fc.currTable);

    taskDef.params->accept(*this);
    taskDef.body->accept(*this);

    fc.currTask = nullptr;
    fc.currTable = oriTable;
}
void SemParser::visitTaskParams(ASTTaskParams& taskParams){
    if(taskParams.inParams) taskParams.inParams->accept(*this);
    if(taskParams.outParams) taskParams.outParams->accept(*this);
}
void SemParser::visitInParamList(ASTInParamList& inParas){
    fc.isParseInparas = true;
    for(const auto& it: inParas.paramDecls){
        it->accept(*this);
    }
    fc.isParseInparas = false;
}
void SemParser::visitOutParamList(ASTOutParamList& outParas){
    for(const auto& it: outParas.paramDecls){
        it->accept(*this);
    }
}
void SemParser::visitParamDecl(ASTParamDecl& paraDecl){
    std::string name = paraDecl.idtk->getContent();
    paraDecl.type->accept(*this);
    if(fc.currTask){
        if(fc.isParseInparas) {
            fc.currTask->bindInPara(name, fc.typeStr);
            fc.currTable->addSymbol(name, std::make_shared<VarSym>(name, fc.typeStr));
        }
        else fc.currTask->bindOutPara(name, fc.typeStr);
    }
}
void SemParser::visitTaskBody(ASTTaskBody& taskBody){
    for(const auto& it: taskBody.stateDecls){
        it->accept(*this);
    }
    taskBody.node->accept(*this);
}
void SemParser::visitStateDecl(ASTStateDecl& stateDecl){
    std::string varName = stateDecl.idtk->getContent();
    std::string varType;
    int line = stateDecl.idtk->getLine();
    stateDecl.type->accept(*this);

    if(fc.currTable->isSymExistHere(varName)){
        fc.errTable.addError(RTDLErr{"Semantic Error", line, "The var: " + varName + " already exists."});
    } else {
        fc.currTable->addSymbol(varName, std::make_shared<VarSym>(varName, fc.typeStr));
    }
}
void SemParser::visitNode(ASTNode& node){
    if(node.comNode) node.comNode->accept(*this);
    if(node.decNode) node.decNode->accept(*this);
    if(node.leafNode) node.leafNode->accept(*this);
}
void SemParser::visitComNode(ASTComNode& comNode){
    if(comNode.selNode) comNode.selNode->accept(*this);
    if(comNode.seqNode) comNode.seqNode->accept(*this);
}
void SemParser::visitDecNode(ASTDecNode& decNode){
    if(decNode.retryNode) decNode.retryNode->accept(*this);
    if(decNode.toutNode) decNode.toutNode->accept(*this);
}
void SemParser::visitLeafNode(ASTLeafNode& leafNode){
    if(leafNode.skicall) leafNode.skicall->accept(*this);
    if(leafNode.taskcall) leafNode.taskcall->accept(*this);
    if(leafNode.waitStmt) leafNode.waitStmt->accept(*this);
    if(leafNode.cckStmt)  leafNode.cckStmt->accept(*this);
}
void SemParser::visitSeqNode(ASTSeqNode& seqNode){
    seqNode.childBlock->accept(*this);
}
void SemParser::visitSelNode(ASTSelNode& selNode){
    selNode.childBlock->accept(*this);
}
void SemParser::visitChildBlock(ASTChildBlock& cb){
    for(const auto& it: cb.nodes) {
        it->accept(*this);
    }
}
void SemParser::visitRetryNode(ASTRetryNode& retryNode){
    retryNode.node->accept(*this);
}
void SemParser::visitToutNode(ASTToutNode& toutNode){
    toutNode.node->accept(*this);
} 
void SemParser::visitSkillCall(ASTSkillCall& skiCall){
    std::string skiName = skiCall.idtk->getContent();
    int line = skiCall.idtk->getLine();
    if(!fc.currTable->isSymExistHierarchy(skiName)){
        fc.errTable.addError(RTDLErr{"Semantic Error", line, "The skill: " + skiName + " doesn't exist."});
    } 
    fc.currSkill = std::dynamic_pointer_cast<SkillSym>(fc.currTable->getSymbol(skiName));
    skiCall.bindsec->accept(*this);
    fc.currSkill = nullptr;
}
void SemParser::visitTaskCall(ASTTaskCall& taskCall){
    std::string taskName = taskCall.idtk->getContent();
    int line = taskCall.idtk->getLine();
    if(!fc.currTable->isSymExistHere(taskName)){
        fc.errTable.addError(RTDLErr{"Semantic Error", line, "The task: " + taskName + " doesn't exist."});
    }
    taskCall.bindsec->accept(*this);
}
void SemParser::visitCheckStmt(ASTCheckStmt& checkStmt){
    checkStmt.condExp->accept(*this);
}
void SemParser::visitWaitStmt(ASTWaitStmt& waitStmt){
    waitStmt.dur->accept(*this);
}
void SemParser::visitBindSec(ASTBindSec& bindSec){
    if(bindSec.inbinds) bindSec.inbinds->accept(*this);
    if(bindSec.outbinds) bindSec.outbinds->accept(*this);
}
void SemParser::visitInBindings(ASTInBindings& inBindings){
    for(const auto& it: inBindings.inbs){
        it->accept(*this);
    }
} 
void SemParser::visitInBinding(ASTInBinding& inBinding){
    std::string inPara = inBinding.idtk->getContent();
    int line = inBinding.idtk->getLine();
    inBinding.value->accept(*this);
    if(fc.currSkill){
        std::string skiInParaType = fc.currSkill->getInType(inPara);
        std::string realParaType = fc.typeStr;
        if(skiInParaType != realParaType){
            fc.errTable.addError(RTDLErr("Semantic Error", line, "The type of the passed in param is: " + realParaType + 
                " and it is incompatible with '" + inPara + "', the type of which is " + skiInParaType + "."));
        }
    }
}
void SemParser::visitOutBindings(ASTOutBindings& outBindings){
    for(const auto& it: outBindings.outbs){
        it->accept(*this);
    }
}
void SemParser::visitOutBinding(ASTOutBinding& outBinding){
    std::string outPara = outBinding.idtk->getContent();
    int line = outBinding.idtk->getLine();
    outBinding.ref->accept(*this);
    if(fc.currSkill){
        std::string skiOutParaType = fc.currSkill->getOutType(outPara);
        std::string realParaType = fc.typeStr;
        if(skiOutParaType != realParaType){
            fc.errTable.addError(RTDLErr("Semantic Error", line, "The type of '" + outBinding.ref->idtk->getContent() + "' is: " + realParaType + 
            " and it is incompatible with '" + outPara + "', the type of which is " + skiOutParaType + "."));
        }
    }

}
void SemParser::visitValue(ASTValue& value){
    if(value.literal) value.literal->accept(*this);
    if(value.ref) value.ref->accept(*this);
}
void SemParser::visitCondExp(ASTCondExp& condExp){
    condExp.lorExp->accept(*this);
}
void SemParser::visitLOrExp(ASTLOrExp& lorExp){
    for(const auto& it: lorExp.landExps){
        it->accept(*this);
    }
}
void SemParser::visitLAndExp(ASTLAndExp& landExp){
    for(const auto& it: landExp.eqExps){
        it->accept(*this);
    }
}
void SemParser::visitEqExp(ASTEqExp& eqExp){
    for(const auto& it: eqExp.relExps){
        it->accept(*this);
    }
}
void SemParser::visitRelExp(ASTRelExp& relExp){
    relExp.addExp1->accept(*this);
    if(relExp.addExp2) relExp.addExp2->accept(*this);
}
void SemParser::visitAddExp(ASTAddExp& addExp){
    for(const auto& it: addExp.mulExps){
        it->accept(*this);
    }
}
void SemParser::visitMulExp(ASTMulExp& mulExp){
    for(const auto& it: mulExp.unaryExps){
        it->accept(*this);
    }
}
void SemParser::visitUnaryExp(ASTUnaryExp& unaryExp){
    if(unaryExp.primaryExp) unaryExp.primaryExp->accept(*this);
    if(unaryExp.unaryExp) unaryExp.unaryExp->accept(*this);
}
void SemParser::visitPrimaryExp(ASTPrimaryExp& priExp){
    if(priExp.condExp) priExp.condExp->accept(*this);
    if(priExp.literal) priExp.literal->accept(*this);
    if(priExp.ref) priExp.ref->accept(*this);
}
void SemParser::visitLiteral(ASTLiteral& literal){
    if(literal.ltype == ASTLiteral::AST_LITERAL_TYPE::BOOL){
        fc.typeStr = "bool";
    } else if(literal.ltype == ASTLiteral::AST_LITERAL_TYPE::DOUBLE){
        fc.typeStr = "double";
    } else if(literal.ltype == ASTLiteral::AST_LITERAL_TYPE::STRING){
        fc.typeStr = "string";
    } else if(literal.ltype == ASTLiteral::AST_LITERAL_TYPE::INT){
        fc.typeStr = "int";
    }
}
void SemParser::visitRef(ASTRef& ref){
    std::string name = ref.idtk->getContent();
    SymbolSPtr sym = fc.currTable->getSymbol(name);
    if(!sym){
        int line = ref.idtk->getLine();
        fc.errTable.addError(RTDLErr("Semantic Error", line, "The ref: " + name + " doesn't exist."));
        return;
    }
    VarSymSPtr var = std::dynamic_pointer_cast<VarSym>(sym);
    fc.typeStr = var->getType();
}
void SemParser::visitDuration(ASTDuration& dur){
    ;
}
void SemParser::visitType(ASTType& type){
    if(type.buildinType) type.buildinType->accept(*this);
    else fc.typeStr = type.selfDefType->getContent();
}
void SemParser::visitBuiltinType(ASTBuiltinType& bitype){
    if(bitype.doubletk) fc.typeStr = "double";
    if(bitype.booltk) fc.typeStr = "bool";
    if(bitype.inttk) fc.typeStr = "int";
    if(bitype.stringtk) fc.typeStr = "string";
}