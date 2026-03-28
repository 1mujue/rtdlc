#include "lexer/Lexer.hpp"

bool isWhiteSpace(int ch){
    return ch == '\n' || ch == '\t' || ch == ' ' || ch == '\r' || ch == '\f' || ch == '\v';
}

TokenSPtr anaIDToken(const std::string& word, int line){
    TokenSPtr token = std::make_shared<Token>();
    token->setLine(line);
    if(word == "true") {
        token->setTE(TOKEN_ENUM::LBTK);
        token->setNumValue(1);
    } else if(word == "false"){
        token->setTE(TOKEN_ENUM::LBTK);
        token->setNumValue(0);
    }
    else {
        if(word == "def") token->setTE(TOKEN_ENUM::DEFTK);
        else if(word == "task") token->setTE(TOKEN_ENUM::TASKTK);
        else if(word == "state") token->setTE(TOKEN_ENUM::STATK);
        else if(word == "sequence") token->setTE(TOKEN_ENUM::SEQTK);
        else if(word == "selector") token->setTE(TOKEN_ENUM::SELTK);
        else if(word == "retry") token->setTE(TOKEN_ENUM::RETRYTK);
        else if(word == "timeout") token->setTE(TOKEN_ENUM::TOUTTK);
        else if(word == "do") token->setTE(TOKEN_ENUM::DOTK);
        else if(word == "call") token->setTE(TOKEN_ENUM::CALLTK);
        else if(word == "check") token->setTE(TOKEN_ENUM::CCKTK);
        else if(word == "wait") token->setTE(TOKEN_ENUM::WAITTK);
        else if(word == "bool") token->setTE(TOKEN_ENUM::BOOLTK);
        else if(word == "int") token->setTE(TOKEN_ENUM::INTTK);
        else if(word == "float") token->setTE(TOKEN_ENUM::FLOATTK);
        else if(word == "string") token->setTE(TOKEN_ENUM::STRTK);
        else token->setTE(TOKEN_ENUM::IDTK);
        token->setContent(word);
    }
    return token;
}
std::vector<TokenSPtr> Lexer::tokenize(){
    std::vector<TokenSPtr> tokens;
    int line = 1; char ch;
    while(!file.eof()){
        file.get(ch);
        while(isWhiteSpace(ch)){
            if(ch == '\n') line++;
            if(file.eof()) break;
            file.get(ch);
        }
        if(file.eof()) break;

        if(ch == '_' || isalpha(ch)){
            std::string word;
            while(ch == '_' || isalnum(ch) || isalpha(ch)){
                word.push_back(ch);
                if(file.eof()) break;
                file.get(ch);
            }
            tokens.push_back(anaIDToken(word, line));
            if(file.eof()) break;
            else file.putback(ch);
        } else if(isdigit(ch)){
            std::string numStr;
            bool isFloat = false;
            while(isdigit(ch) || ch == '.'){
                if(ch == '.') isFloat = true;
                numStr.push_back(ch);
                if(file.eof()) break;
                file.get(ch);
            }
            TokenSPtr token = std::make_shared<Token>(isFloat? TOKEN_ENUM::LFTK: TOKEN_ENUM::LITK, line);
            token->setNumValue(std::stod(numStr));
            tokens.push_back(token);

            if(file.eof()) break;
            else file.putback(ch);
        } else if(ch == ';'){
            tokens.push_back(std::make_shared<Token>(TOKEN_ENUM::SEMITK, line, ";"));
        } else if(ch == ':'){
            tokens.push_back(std::make_shared<Token>(TOKEN_ENUM::COLTK, line, ":"));
        } else if(ch == '{'){
            tokens.push_back(std::make_shared<Token>(TOKEN_ENUM::LBRETK, line, "{")); 
        } else if(ch == '}'){
            tokens.push_back(std::make_shared<Token>(TOKEN_ENUM::RBRETK, line, "}"));
        } else if(ch == '('){
            tokens.push_back(std::make_shared<Token>(TOKEN_ENUM::LPARTK, line, "("));
        } else if(ch == ')'){
            tokens.push_back(std::make_shared<Token>(TOKEN_ENUM::RPARTK, line, ")"));
        } else if(ch == ','){
            tokens.push_back(std::make_shared<Token>(TOKEN_ENUM::COMTK, line, ","));
        } else if(ch == '='){
            if(file.peek() == '='){
                file.get();
                tokens.push_back(std::make_shared<Token>(TOKEN_ENUM::EQLTK, line, "=="));
            } else {
                tokens.push_back(std::make_shared<Token>(TOKEN_ENUM::ASGNTK, line, "="));
            }
        } else if(ch == '-'){
            // check for '->'
            if(file.peek() == '>'){
                file.get();
                tokens.push_back(std::make_shared<Token>(TOKEN_ENUM::ARWTK, line, "->"));
            } else {
                throw std::runtime_error("Unexpected character: " + std::string(1, ch) + " at line " + std::to_string(line));
            }
        } else if(ch == '|'){
            // check for '||'
            if(file.peek() == '|'){
                file.get();
                tokens.push_back(std::make_shared<Token>(TOKEN_ENUM::ORTK, line, "||"));
            } else {
                throw std::runtime_error("Unexpected character: " + std::string(1, ch) + " at line " + std::to_string(line));
            }
        } else if(ch == '&'){
            // check for '&&'
            if(file.peek() == '&'){
                file.get();
                tokens.push_back(std::make_shared<Token>(TOKEN_ENUM::ANDTK, line, "&&"));
            } else {
                throw std::runtime_error("Unexpected character: " + std::string(1, ch) + " at line " + std::to_string(line));
            }
        } else if(ch == '!'){
            if(file.peek() == '='){
                file.get();
                tokens.push_back(std::make_shared<Token>(TOKEN_ENUM::NEQTK, line, "!="));
            } else {
                tokens.push_back(std::make_shared<Token>(TOKEN_ENUM::NOTTK, line, "!"));
            }
        } else if(ch == '>'){
            if(file.peek() == '='){
                file.get();
                tokens.push_back(std::make_shared<Token>(TOKEN_ENUM::GEQTK, line, ">="));
            } else {
                tokens.push_back(std::make_shared<Token>(TOKEN_ENUM::GRTTK, line, ">"));
            }
        } else if(ch == '<'){
            if(file.peek() == '='){
                file.get();
                tokens.push_back(std::make_shared<Token>(TOKEN_ENUM::LEQTK, line, "<="));
            } else {
                tokens.push_back(std::make_shared<Token>(TOKEN_ENUM::LSSTK, line, "<"));
            }
        } else if(ch == '"'){
            std::string str;
            // str.push_back('\"');
            while(true){
                char c = file.get();
                if(c == '"') break;
                str.push_back(c);
            }
            // str.push_back('\"');
            tokens.push_back(std::make_shared<Token>(TOKEN_ENUM::LSTK, line, str));
        }
    }
    return tokens;
}