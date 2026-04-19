#pragma once
#include "forward.hpp"
#include <string>
#include <vector>
#include <iostream>
class RTDLErr{
private:
    std::string type;
    int line;
    std::string content;
public:
    RTDLErr(const std::string& errType, int line, const std::string& content) : type(errType), line(line), content(content){}
    void print(std::ostream& o) const {
        o << type << " at line " << line << ": " << content << '\n';
    }
};

class RTDLErrTable{
private:
    std::vector<RTDLErr> errs;
public:
    bool isErr(){
        return !errs.empty();
    }
    void addError(const RTDLErr& err) {
        errs.push_back(err);
    }
    void printErr(std::ostream& o) const {
        for(const auto& it: errs) it.print(o);
    }
};