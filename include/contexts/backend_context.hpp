#pragma once
#include <string>
#include <iostream>
class BackendContext{
public:
    // bt_node_gen
    std::string className;
    std::string ros2WS;
    std::string ros2Pkg;
    std::string ReqTypeInCPP;
    std::ostream* hpp_out_ = nullptr, *cpp_out_ = nullptr;
private:
    BackendContext(){}
    ~BackendContext(){}
    BackendContext(const BackendContext&) = delete;
    const BackendContext& operator=(const BackendContext&) = delete;
public:
    std::ostream& getHPPOut(){
        return *hpp_out_;
    }
    std::ostream& getCPPOut(){
        return *cpp_out_;
    }
    static BackendContext& getInstance(){
        static BackendContext bc;
        return bc;
    }
};