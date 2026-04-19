#include "lexer/entities.hpp"

const char* toString(TOKEN_ENUM te){
    switch (te) {
        case TOKEN_ENUM::IDTK:    return "IDTK";
        case TOKEN_ENUM::DEFTK:   return "DEFTK";
        case TOKEN_ENUM::TASKTK:  return "TASKTK";
        case TOKEN_ENUM::STATK:   return "STATK";
        case TOKEN_ENUM::SEQTK:   return "SEQTK";
        case TOKEN_ENUM::SELTK:   return "SELTK";
        case TOKEN_ENUM::RETRYTK: return "RETRYTK";
        case TOKEN_ENUM::TOUTTK:  return "TOUTTK";
        case TOKEN_ENUM::DOTK:    return "DOTK";
        case TOKEN_ENUM::CALLTK:  return "CALLTK";
        case TOKEN_ENUM::CCKTK:   return "CCKTK";
        case TOKEN_ENUM::WAITTK:  return "WAITTK";
        case TOKEN_ENUM::SEMITK:  return "SEMITK";
        case TOKEN_ENUM::COLTK:   return "COLTK";
        case TOKEN_ENUM::LBRETK:  return "LBRETK";
        case TOKEN_ENUM::RBRETK:  return "RBRETK";
        case TOKEN_ENUM::LPARTK:  return "LPARTK";
        case TOKEN_ENUM::RPARTK:  return "RPARTK";
        case TOKEN_ENUM::COMTK:   return "COMTK";
        case TOKEN_ENUM::ASGNTK:  return "ASGNTK";
        case TOKEN_ENUM::ARWTK:   return "ARWTK";
        case TOKEN_ENUM::ORTK:    return "ORTK";
        case TOKEN_ENUM::ANDTK:   return "ANDTK";
        case TOKEN_ENUM::NOTTK:   return "NOTTK";
        case TOKEN_ENUM::EQLTK:   return "EQLTK";
        case TOKEN_ENUM::NEQTK:   return "NEQTK";
        case TOKEN_ENUM::GRTTK:   return "GRTTK";
        case TOKEN_ENUM::GEQTK:   return "GEQTK";
        case TOKEN_ENUM::LSSTK:   return "LSSTK";
        case TOKEN_ENUM::LEQTK:   return "LEQTK";
        case TOKEN_ENUM::BOOLTK:  return "BOOLTK";
        case TOKEN_ENUM::INTTK:   return "INTTK";
        case TOKEN_ENUM::DOUBLETK: return "DOUBLETK";
        case TOKEN_ENUM::STRTK:   return "STRTK";
        case TOKEN_ENUM::LITK:    return "LITK";
        case TOKEN_ENUM::LFTK:    return "LFTK";
        case TOKEN_ENUM::LBTK:    return "LBTK";
        case TOKEN_ENUM::LSTK:    return "LSTK";
        case TOKEN_ENUM::EMPTY:   return "EMPTY";
    }
    return "UNKNOWN_TOKEN_ENUM";
}
std::ostream& operator<<(std::ostream& os, TOKEN_ENUM te) {
    return os << toString(te);
}