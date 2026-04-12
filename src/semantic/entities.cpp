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
std::string Symbol::getName(){return name;}
Symbol::~Symbol(){}
void Symbol::print(std::ostream& o) {
    o << se << ": " << name;
}
void SymbolTable::setFaTable(SymbolTableSPtr faTable){
    faTable = faTable;
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