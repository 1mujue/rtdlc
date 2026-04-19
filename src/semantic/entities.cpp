#include "semantic/entities.hpp"

const char* toString(SYM_ENUM se){
    switch (se)
    {
    case SYM_ENUM::SKILL_SYM:   return      "Skill Symbol";
    case SYM_ENUM::TASK_SYM:    return      "Task Symbol";
    case SYM_ENUM::VAR_SYM:     return      "Var Symbol";
    default:                    return      "Unknown SYM_ENUM";
    }
}
std::ostream& operator<<(std::ostream& o, SYM_ENUM se){
    return o << toString(se);
}

Symbol::Symbol(SYM_ENUM se, const std::string& name) : se(se), name(name){}
SYM_ENUM Symbol::getSE(){return se;}
std::string Symbol::getName(){return name;}
Symbol::~Symbol(){}
void Symbol::print(std::ostream& o) {
    o << se << ": " << name;
}
void SymbolTable::setFaTable(SymbolTableSPtr faTable){
    this->faTable = faTable;
}
void SymbolTable::addChildTable(SymbolTableSPtr childTable){
    childTables.push_back(childTable);
}
void SymbolTable::addSymbol(const std::string& name, SymbolSPtr symbol){
    symbols[name] = symbol;
}
bool SymbolTable::isSymExistHere(const std::string& name){
    return symbols.find(name) != symbols.end();
}
bool SymbolTable::isSymExistHierarchy(const std::string& name){
    return isSymExistHere(name) || (faTable && faTable->isSymExistHierarchy(name));
}
SymbolSPtr SymbolTable::getSymbol(const std::string& name){
    if(!isSymExistHere(name)) return faTable? faTable->getSymbol(name): nullptr;
    return symbols.find(name)->second;
}
const std::unordered_map<std::string, SymbolSPtr>& SymbolTable::getAllSymbols(){
    return symbols;
}
void SymbolTable::print(std::ostream& o){
    for(const auto& it: symbols){
        it.second->print(o);
    }
}

void Callable::bindInPara(const std::string& name, const std::string& type){
    inParams[name] = type;
}
void Callable::bindOutPara(const std::string& name, const std::string& type){
    outParams[name] = type;
}
std::string Callable::getInType(const std::string& name) {return inParams[name];}
std::string Callable::getOutType(const std::string& name) {return outParams[name];}
const std::unordered_map<std::string, std::string>& Callable::getInParas(){ return inParams; }
const std::unordered_map<std::string, std::string>& Callable::getOutParas(){ return outParams; }
Callable::Callable(SYM_ENUM se, const std::string& name): Symbol(se, name){}
void Callable::print(std::ostream& o) {
    Symbol::print(o);
    o << '\n';
    o << "InParams: " << '\n';
    for(const auto& it: inParams){
        o << "name: " << it.first << ", type: " << it.second << '\n';
    }
    o << "OutParams: " << '\n';
    for(const auto& it: outParams){
        o << "name: " << it.first << ", type: " << it.second << '\n';
    }
}

SkillSym::SkillSym(const std::string& name): Callable(SYM_ENUM::SKILL_SYM, name){}
std::string SkillSym::getROS2Backend(){
    return rosBackend;
}
void SkillSym::setROS2Backend(const std::string& ros2Backend){
    this->rosBackend = ros2Backend;
}
void SkillSym::setROS2ReqType(const std::string& ros2_req_type){
    this->ros2ReqType = ros2_req_type;
}
void SkillSym::setROS2Srv(const std::string& ros2SrvName){
    this->ros2SrvName = ros2SrvName;
}
std::string SkillSym::getROS2ReqType(){
    return ros2ReqType;
}
std::string SkillSym::getROS2Srv(){
    return this->ros2SrvName;
}
void SkillSym::addInputToROSMapping(const std::string& port, const std::string& target) {
    inputToRosMappings.push_back({port, target});
}

void SkillSym::addOutputToROSMapping(const std::string& source, const std::string& port) {
    outputToRosMappings.push_back({source, port});
}

const std::vector<InputToRosMapping>& SkillSym::getInputToROSMappings() const {
    return inputToRosMappings;
}

const std::vector<OutputToRosMapping>& SkillSym::getOutputToROSMappings() const {
    return outputToRosMappings;
}
void SkillSym::print(std::ostream& o){
    Callable::print(o);
}

TaskSym::TaskSym(const std::string& name): Callable(SYM_ENUM::TASK_SYM, name){}
void TaskSym::print(std::ostream& o){
    Callable::print(o);
}

std::string VarSym::getType(){ return type; }
VarSym::VarSym(const std::string& name, const std::string& type): Symbol(SYM_ENUM::VAR_SYM, name), type(type){}
void VarSym::print(std::ostream& o){
    Symbol::print(o);
    o << " type: " << type << '\n';
}