#include <iostream>
#include <fstream>
#include "lexer/lexer.hpp"
#include "syntactic/syn_parser.hpp"
#include "semantic/sem_parser.hpp"
#include "semantic/entities.hpp"
#include "contexts/frontend_context.hpp"
#include "contexts/backend_context.hpp"
#include "bt_xml_gen/xml_emitter.hpp"
#include "bt_node_gen/node_emitter.hpp"

struct Param{
    std::string key;
    std::string value;
};
void handleParas(int argc, char** argv, std::vector<Param>& paras){
    for(int i = 1; i < argc; i += 2){
        for(auto& it: paras){
            if(argv[i] == it.key){
                if(i + 1 >= argc){
                    throw std::runtime_error("Param Error: lack value of " + it.key);
                }
                it.value = argv[i + 1];
            }
        }
    }
}
void checkParas(int argc, char** argv, std::vector<Param>& paras){
    for(const auto& it: paras){
        if(it.value.empty()){
            throw std::runtime_error("Param Error: lack value of " + it.key);
        }
    }
}

FrontendContext& fc = FrontendContext::getInstance();
BackendContext& bc = BackendContext::getInstance();

int main(int argc, char** argv){

    Param rtdlPara{"--rtdl-in", "rtdl.txt"}, lexerPara{"--lexer-out", "lexer.txt"}, 
    synPara{"--syn-out", "syntactic.txt"}, semPara{"--sem-out","semantic.txt"},
    btPara{"--bt-out", "tree.xml"}, errPara{"--error-out", "error.txt"},
    ros2WSPara{"--ros2-ws", ""}, ros2PkgPara{"--ros2-pkg", "rtdl_demo_bt_test"};
    std::vector<Param> paras{rtdlPara, lexerPara, synPara, semPara, btPara, errPara, ros2WSPara, ros2PkgPara};
    handleParas(argc, argv, paras);
    checkParas(argc, argv, paras);

    bc.ros2WS = paras[6].value;
    bc.ros2Pkg = paras[7].value;

    std::cout << "rtdl input file: " << paras[0].value << '\n';
    std::cout << "bt output file: " << paras[4].value << '\n';
    std::ofstream lexerOut(paras[1].value), synOut(paras[2].value), 
    semOut(paras[3].value), xmlOut(paras[4].value),
    errOut(paras[5].value);

    Lexer lexer(paras[0].value);
    std::vector<std::shared_ptr<Token>> tokens = lexer.tokenize();
    for(const auto& token: tokens) token->print(lexerOut);

    SynParser synParser;
    ASTCompUnitSPtr astCompUnit = synParser.parse(tokens);
    astCompUnit->print(synOut);

    SemParser SemParser;
    SemParser.parse(astCompUnit);
    fc.globTable->print(semOut);

    if(fc.errTable.isErr()){
        std::cerr << "Some errors happened when compiling, it will be displayed here while also saved to " <<  paras[5].value << '\n';
        fc.errTable.printErr(errOut);
        fc.errTable.printErr(std::cerr);
    }  else {
        XMLEmitter xmlEmi;
        XMLNodeSPtr XMLCompUnit = xmlEmi.emitXML(astCompUnit);
        XMLCompUnit->print(xmlOut, 0); 

        NodeEmitter nodeEmi;
        nodeEmi.emitNode();
    }
    return 0;
}
