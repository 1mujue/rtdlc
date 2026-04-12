#include <iostream>
#include <fstream>
#include "lexer/lexer.hpp"
#include "syntactic/syn_parser.hpp"
#include "semantic/sem_parser.hpp"
#include "semantic/entities.hpp"
#include "xml_gen/xml_emitter.hpp"
#include "contexts/frontend_context.hpp"

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

FrontendContext& fc = FrontendContext::getInstance();

int main(int argc, char** argv){

    Param rtdlPara{"--rtdl-in", "rtdl.txt"}, lexerPara{"--lexer-out", "lexer.txt"}, 
    synPara{"--syn-out", "syntactic.txt"}, semPara{"--sem-out","semantic.txt"},
    btPara{"--bt-out", "tree.xml"}, errPara{"--error-out", "error.txt"};
    std::vector<Param> paras{rtdlPara, lexerPara, synPara, semPara, btPara, errPara};
    handleParas(argc, argv, paras);

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

    XMLEmitter xmlEmi;
    XMLNodeSPtr XMLCompUnit = xmlEmi.emitXML(astCompUnit);
    XMLCompUnit->print(xmlOut, 0); 

    fc.errTable.printErr(errOut);
    
    return 0;
}
