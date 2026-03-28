#include <iostream>
#include <fstream>
#include "lexer/Lexer.hpp"
#include "syntactic/syn_parser.hpp"
#include "xml_gen/xml_emitter.hpp"

struct Param{
    std::string key;
    std::string value;
};
void handleParas(int argc, char** argv, std::vector<Param>& paras){
    for(int i = 0; i < argc; i += 2){
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
int main(int argc, char** argv){

    Param rtdlPara{"--rtdl-in", "rtdl.txt"}, lexerPara{"--lexer-out", "lexer.txt"}, synPara{"--syn-out", "syntactic.txt"}, btPara{"--bt-out", "tree.xml"};
    std::vector<Param> paras{rtdlPara, lexerPara, synPara, btPara};
    handleParas(argc, argv, paras);

    std::ofstream lexerOut(paras[1].value), synOut(paras[2].value), xmlOut(paras[3].value);

    Lexer lexer(rtdlPara.value);
    std::vector<std::shared_ptr<Token>> tokens = lexer.tokenize();
    for(const auto& token: tokens) token->print(lexerOut);

    SynParser synParser;
    ASTCompUnitSPtr astCompUnit = synParser.parse(tokens);
    astCompUnit->print(synOut);

    XMLEmitter xmlEmi;
    XMLNodeSPtr XMLCompUnit = xmlEmi.emitXML(astCompUnit);
    XMLCompUnit->print(xmlOut, 0); 
    return 0;
}
