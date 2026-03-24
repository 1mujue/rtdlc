#pragma once
#include <fstream>
#include <vector>
#include <memory>
#include "lexer/entities.hpp"
class Lexer{
private:
    std::ifstream file;
public:
    Lexer(const std::string& filename) : file(filename) {}
    std::vector<std::shared_ptr<Token>> tokenize();  
};