#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include "forward.hpp"

enum class SYM_ENUM{
    SKILL_SYM,
    TASK_SYM,
    VAR_SYM
};

const char* toString(SYM_ENUM);
std::ostream& operator<<(std::ostream&, SYM_ENUM);

class Symbol{
protected:
    std::string name;
    SYM_ENUM se;
public:
    Symbol(SYM_ENUM se, const std::string& name);
    SYM_ENUM getSE();
    std::string getName();
    virtual ~Symbol();
    virtual void print(std::ostream& o);
};

class Callable : public Symbol{
protected:
    std::unordered_map<std::string, std::string> inParams;
    std::unordered_map<std::string, std::string> outParams;
public:
    void bindInPara(const std::string& name, const std::string& type);
    void bindOutPara(const std::string& name, const std::string& type);
    std::string getInType(const std::string& name);
    std::string getOutType(const std::string& name);
    const std::unordered_map<std::string, std::string>& getInParas();
    const std::unordered_map<std::string, std::string>& getOutParas();
    Callable(SYM_ENUM se, const std::string& name);
    virtual void print(std::ostream& o);
};

struct InputToRosMapping {
    std::string port;    // robot_name
    std::string target;  // request.robot_name
};

struct OutputToRosMapping {
    std::string source;  // response.success
    std::string port;    // success
};

class SkillSym: public Callable{
private:
    std::string rosBackend;
    std::string ros2ReqType;
    std::string ros2SrvName;
    std::vector<InputToRosMapping> inputToRosMappings;
    std::vector<OutputToRosMapping> outputToRosMappings;
public:
    SkillSym(const std::string& name);
    void setROS2Backend(const std::string&);
    void setROS2ReqType(const std::string&);
    void setROS2Srv(const std::string&);
    std::string getROS2Backend();
    std::string getROS2ReqType();
    std::string getROS2Srv();
    void addInputToROSMapping(const std::string& port, const std::string& target);
    void addOutputToROSMapping(const std::string& source, const std::string& port);
    const std::vector<InputToRosMapping>& getInputToROSMappings() const;
    const std::vector<OutputToRosMapping>& getOutputToROSMappings() const;
    virtual void print(std::ostream& o);
};

class TaskSym: public Callable{
public:
    TaskSym(const std::string& name);
    virtual void print(std::ostream& o);
};

class VarSym: public Symbol{
private:
    std::string type;
public:
    std::string getType();
    VarSym(const std::string& name, const std::string& type);
    virtual void print(std::ostream& o);
};

class SymbolTable{
private:
    std::unordered_map<std::string, SymbolSPtr> symbols;
    SymbolTableSPtr faTable = nullptr;
    std::vector<SymbolTableSPtr> childTables;
public:
    void setFaTable(SymbolTableSPtr faTable);
    void addChildTable(SymbolTableSPtr childTable);
    void addSymbol(const std::string& name, SymbolSPtr symbol);
    bool isSymExistHere(const std::string& name);
    bool isSymExistHierarchy(const std::string& name);
    SymbolSPtr getSymbol(const std::string& name);
    const std::unordered_map<std::string, SymbolSPtr>& getAllSymbols();
    void print(std::ostream& o);
};