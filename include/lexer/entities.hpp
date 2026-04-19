#pragma once
#include <string>
#include <iostream>
#include <memory>
enum class TOKEN_ENUM{
    IDTK,
    DEFTK,
    TASKTK,
    STATK,
    SEQTK,
    SELTK,
    RETRYTK,
    TOUTTK,
    DOTK,
    CALLTK,
    CCKTK,
    WAITTK,
    SEMITK,
    COLTK,
    LBRETK,
    RBRETK,
    LPARTK,
    RPARTK,
    COMTK,
    ASGNTK,
    ARWTK,
    ORTK,
    ANDTK,
    PLUSTK,
    MINUSTK,
    MULTK,
    DIVTK,
    MODTK,
    NOTTK,
    EQLTK,
    NEQTK,
    GRTTK,
    GEQTK,
    LSSTK,
    LEQTK,
    BOOLTK,
    INTTK,
    DOUBLETK,
    STRTK,
    LITK,
    LFTK,
    LBTK,
    LSTK,
    EMPTY
};
const char* toString(TOKEN_ENUM te);
std::ostream& operator<<(std::ostream& os, TOKEN_ENUM token);

class Token{
private:
    TOKEN_ENUM te;
    std::string content;
    double numValue = 0.0;
    int line;
public:
    Token(TOKEN_ENUM te) : te(te){}
    Token(TOKEN_ENUM te, int line) : te(te), line(line){}
    Token(TOKEN_ENUM te, int line, const std::string& content) : te(te), line(line), content(content){}
    Token() : te(TOKEN_ENUM::EMPTY), content(""){}
    TOKEN_ENUM getTE() const { return te; }
    std::string getContent() const { return content; }
    double getNumValue() const { return numValue; }
    int getLine() const { return line; }
    void setLine(int line) { this->line = line; }
    void setTE(TOKEN_ENUM te) { this->te = te; }
    void setContent(const std::string& content) { this->content = content; }
    void setNumValue(double numValue) { this->numValue = numValue; }
    void print(std::ostream& out) const {
        out << "Token(Type: " << te << ", Line: " << line;
        if(te == TOKEN_ENUM::LBTK || te == TOKEN_ENUM::LFTK || te == TOKEN_ENUM::LITK){
            out << ", NumValue: " << numValue;
        } else {
            out << ", Content: " << content;
        }
        out << ")\n";
    }
};
using TokenSPtr = std::shared_ptr<Token>;