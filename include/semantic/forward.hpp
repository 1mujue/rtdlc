#pragma once
#include <memory>

class Symbol;
class SymbolTable;
class Callable;
class SkillSym;
class TaskSym;
class VarSym;

using SymbolSPtr = std::shared_ptr<Symbol>;
using SymbolTableSPtr = std::shared_ptr<SymbolTable>;
using CallableSPtr = std::shared_ptr<Callable>;
using SkillSymSPtr = std::shared_ptr<SkillSym>;
using TaskSymSPtr = std::shared_ptr<TaskSym>;
using VarSymSPtr = std::shared_ptr<VarSym>;