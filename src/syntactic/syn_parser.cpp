#include "syntactic/syn_parser.hpp"
#include "syntactic/entities.hpp"

TOKEN_ENUM SynParser::peekToken(){
    return index >= tks.size() ? TOKEN_ENUM::EMPTY : tks[index]->getTE();
}
TokenSPtr SynParser::getTokenWithExcep(TOKEN_ENUM te){
    if(peekToken() != te) throw std::runtime_error("Syntactic Error: token at " + std::to_string(index) +" doesn't match with " + toString(te));
    TokenSPtr ret = tks[index++];
    return ret;
}
TokenSPtr SynParser::getTokenWithoutExcep(TOKEN_ENUM te){
    if(peekToken() != te) return nullptr;
    TokenSPtr ret = tks[index++];
    return ret;
}

void SynParser::parseCompUnit(){
    ASTCompUnitSPtr compUnit = std::make_shared<ASTCompUnit>();
    while(peekToken() == TOKEN_ENUM::DEFTK){
        parseTaskDef();
        compUnit->taskDefs.push_back(std::dynamic_pointer_cast<ASTTaskDef>(astStacks.back()));
        astStacks.pop_back();
    }
    astStacks.push_back(compUnit);
}

void SynParser::parseTaskDef(){
    ASTTaskDefSPtr taskDef = std::make_shared<ASTTaskDef>();
    taskDef->deftk = getTokenWithExcep(TOKEN_ENUM::DEFTK);
    taskDef->tasktk = getTokenWithExcep(TOKEN_ENUM::TASKTK);
    taskDef->idtk = getTokenWithExcep(TOKEN_ENUM::IDTK);
    taskDef->lpartk = getTokenWithExcep(TOKEN_ENUM::LPARTK);
    
    parseTaskParams();
    taskDef->params = std::dynamic_pointer_cast<ASTTaskParams>(astStacks.back());
    astStacks.pop_back();

    taskDef->rpartk = getTokenWithExcep(TOKEN_ENUM::RPARTK);

    parseTaskBody();
    taskDef->body = std::dynamic_pointer_cast<ASTTaskBody>(astStacks.back());
    astStacks.pop_back();

    astStacks.push_back(taskDef);
}

void SynParser::parseTaskParams(){
    ASTTaskParamsSPtr params = std::make_shared<ASTTaskParams>();
    if(peekToken() == TOKEN_ENUM::IDTK) {
        parseInParamList();
        params->inParams = std::dynamic_pointer_cast<ASTInParamList>(astStacks.back());
        astStacks.pop_back();
    }
    params->semitk = getTokenWithExcep(TOKEN_ENUM::SEMITK);
    if(peekToken() == TOKEN_ENUM::IDTK){
        parseOutParamList();
        params->outParams = std::dynamic_pointer_cast<ASTOutParamList>(astStacks.back());
        astStacks.pop_back();
    }
    astStacks.push_back(params);
}

void SynParser::parseInParamList(){
    ASTInParamListSPtr inParams = std::make_shared<ASTInParamList>();
    parseParamDecl();
    inParams->paramDecls.push_back(std::dynamic_pointer_cast<ASTParamDecl>(astStacks.back()));
    astStacks.pop_back();
    while(peekToken() == TOKEN_ENUM::COMTK){
        inParams->commatks.push_back(getTokenWithExcep(TOKEN_ENUM::COMTK));
        parseParamDecl();
        inParams->paramDecls.push_back(std::dynamic_pointer_cast<ASTParamDecl>(astStacks.back()));
        astStacks.pop_back();
    }
    astStacks.push_back(inParams);
}

void SynParser::parseOutParamList(){
    ASTOutParamListSPtr outParams = std::make_shared<ASTOutParamList>();
    parseParamDecl();
    outParams->paramDecls.push_back(std::dynamic_pointer_cast<ASTParamDecl>(astStacks.back()));
    astStacks.pop_back();
    while(peekToken() == TOKEN_ENUM::COMTK){
        outParams->commatks.push_back(getTokenWithExcep(TOKEN_ENUM::COMTK));
        parseParamDecl();
        outParams->paramDecls.push_back(std::dynamic_pointer_cast<ASTParamDecl>(astStacks.back()));
        astStacks.pop_back();
    }
    astStacks.push_back(outParams);
}

void SynParser::parseParamDecl(){
    ASTParamDeclSPtr paramDecl = std::make_shared<ASTParamDecl>();
    paramDecl->idtk = getTokenWithExcep(TOKEN_ENUM::IDTK);
    paramDecl->coltk = getTokenWithExcep(TOKEN_ENUM::COLTK);
    parseType();
    paramDecl->type = std::dynamic_pointer_cast<ASTType>(astStacks.back());
    astStacks.pop_back();
    astStacks.push_back(paramDecl);
}

void SynParser::parseTaskBody(){
    ASTTaskBodySPtr taskBody = std::make_shared<ASTTaskBody>();
    taskBody->lbretk = getTokenWithExcep(TOKEN_ENUM::LBRETK);
    while(peekToken() == TOKEN_ENUM::STATK){
        parseStateDecl();
        taskBody->stateDecls.push_back(std::dynamic_pointer_cast<ASTStateDecl>(astStacks.back()));
        astStacks.pop_back();
    }
    parseNode();
    taskBody->node = std::dynamic_pointer_cast<ASTNode>(astStacks.back());
    astStacks.pop_back();
    taskBody->rbretk = getTokenWithExcep(TOKEN_ENUM::RBRETK);
    astStacks.push_back(taskBody);
}

void SynParser::parseStateDecl(){
    ASTStateDeclSPtr stateDecl = std::make_shared<ASTStateDecl>();
    stateDecl->stattk = getTokenWithExcep(TOKEN_ENUM::STATK);
    stateDecl->idtk = getTokenWithExcep(TOKEN_ENUM::IDTK);
    stateDecl->coltk = getTokenWithExcep(TOKEN_ENUM::COLTK);
    parseType();
    stateDecl->type = std::dynamic_pointer_cast<ASTType>(astStacks.back());
    astStacks.pop_back();
    stateDecl->semitk = getTokenWithExcep(TOKEN_ENUM::SEMITK);
    astStacks.push_back(stateDecl);
}

void SynParser::parseNode(){
    ASTNodeSPtr node = std::make_shared<ASTNode>();
    TOKEN_ENUM te = peekToken();
    if(te == TOKEN_ENUM::SEQTK || te == TOKEN_ENUM::SELTK){
        parseCompositeNode();
        node->comNode = std::dynamic_pointer_cast<ASTComNode>(astStacks.back());
        astStacks.pop_back();
    } else if(te == TOKEN_ENUM::RETRYTK || te == TOKEN_ENUM::TOUTTK){
        parseDecoratorNode();
        node->decNode = std::dynamic_pointer_cast<ASTDecNode>(astStacks.back());
        astStacks.pop_back();
    } else if(te == TOKEN_ENUM::CALLTK || te == TOKEN_ENUM::DOTK || te == TOKEN_ENUM::CCKTK || te == TOKEN_ENUM::WAITTK){
        parseLeafNode();
        node->leafNode = std::dynamic_pointer_cast<ASTLeafNode>(astStacks.back());
        astStacks.pop_back();
    } else {
        throw std::runtime_error("Syntactic Error: can't parse Node.");
    }
    astStacks.push_back(node);
}  

void SynParser::parseCompositeNode(){
    ASTComNodeSPtr comNode = std::make_shared<ASTComNode>();
    TOKEN_ENUM te = peekToken();
    if(te == TOKEN_ENUM::SEQTK){
        parseSequenceNode();
        comNode->seqNode = std::dynamic_pointer_cast<ASTSeqNode>(astStacks.back());
        astStacks.pop_back();
    } else if(te == TOKEN_ENUM::SELTK){
        parseSelectorNode();
        comNode->selNode = std::dynamic_pointer_cast<ASTSelNode>(astStacks.back());
        astStacks.pop_back();
    } else{
        throw std::runtime_error("Syntactic Error: can't parse Composite Node.");
    }
    astStacks.push_back(comNode);
}

void SynParser::parseDecoratorNode(){
    ASTDecNodeSPtr decNode = std::make_shared<ASTDecNode>();
    TOKEN_ENUM te = peekToken();
    if(te == TOKEN_ENUM::RETRYTK){
        parseRetryNode();
        decNode->retryNode = std::dynamic_pointer_cast<ASTRetryNode>(astStacks.back());
        astStacks.pop_back();
    } else if(te == TOKEN_ENUM::TOUTTK){
        parseTimeoutNode();
        decNode->toutNode = std::dynamic_pointer_cast<ASTToutNode>(astStacks.back());
        astStacks.pop_back();
    } else {
        throw std::runtime_error("Syntactic Error: can't parse Decorator Node.");
    }
    astStacks.push_back(decNode);
}

void SynParser::parseLeafNode(){
    ASTLeafNodeSPtr leafNode = std::make_shared<ASTLeafNode>();
    TOKEN_ENUM te = peekToken();
    if(te == TOKEN_ENUM::DOTK){
        parseSkillCall();
        leafNode->skicall = std::dynamic_pointer_cast<ASTSkillCall>(astStacks.back());
        astStacks.pop_back();
    }else if(te == TOKEN_ENUM::CALLTK){
        parseTaskCall();
        leafNode->taskcall = std::dynamic_pointer_cast<ASTTaskCall>(astStacks.back());
        astStacks.pop_back();
    } else if(te == TOKEN_ENUM::CCKTK){
        parseCheckStmt();
        leafNode->cckStmt = std::dynamic_pointer_cast<ASTCheckStmt>(astStacks.back());
        astStacks.pop_back();
    } else if(te == TOKEN_ENUM::WAITTK){
        parseWaitStmt();
        leafNode->waitStmt = std::dynamic_pointer_cast<ASTWaitStmt>(astStacks.back());
        astStacks.pop_back();
    } else {
        throw std::runtime_error("Syntactic Error: can't parse Leaf Node.");
    }
    astStacks.push_back(leafNode);
}

void SynParser::parseSequenceNode(){
    ASTSeqNodeSPtr seqNode = std::make_shared<ASTSeqNode>();
    seqNode->seqtk = getTokenWithExcep(TOKEN_ENUM::SEQTK);
    parseChildBlock();
    seqNode->childBlock = std::dynamic_pointer_cast<ASTChildBlock>(astStacks.back());
    astStacks.pop_back();
    astStacks.push_back(seqNode);
}

void SynParser::parseSelectorNode(){
    ASTSelNodeSPtr selNode = std::make_shared<ASTSelNode>();
    selNode->seltk = getTokenWithExcep(TOKEN_ENUM::SELTK);
    parseChildBlock();
    selNode->childBlock = std::dynamic_pointer_cast<ASTChildBlock>(astStacks.back());
    astStacks.pop_back();
    astStacks.push_back(selNode);
}

void SynParser::parseChildBlock(){
    ASTChildBlockSPtr block = std::make_shared<ASTChildBlock>();
    block->lbretk = getTokenWithExcep(TOKEN_ENUM::LBRETK);
    parseNode();
    block->nodes.push_back(std::dynamic_pointer_cast<ASTNode>(astStacks.back()));
    astStacks.pop_back();
    while(peekToken() != TOKEN_ENUM::RBRETK){
        parseNode();
        block->nodes.push_back(std::dynamic_pointer_cast<ASTNode>(astStacks.back()));
        astStacks.pop_back();
    }
    block->rbretk = getTokenWithExcep(TOKEN_ENUM::RBRETK);
    astStacks.push_back(block);
}

void SynParser::parseRetryNode(){
    ASTRetryNodeSPtr retryNode = std::make_shared<ASTRetryNode>();
    retryNode->retrytk = getTokenWithExcep(TOKEN_ENUM::RETRYTK);
    retryNode->lpartk = getTokenWithExcep(TOKEN_ENUM::LPARTK);
    TokenSPtr valToken = getTokenWithExcep(TOKEN_ENUM::LITK);
    retryNode->val = valToken->getNumValue();
    retryNode->rpartk = getTokenWithExcep(TOKEN_ENUM::RPARTK);
    parseNode();
    retryNode->node = std::dynamic_pointer_cast<ASTNode>(astStacks.back());
    astStacks.pop_back();
    astStacks.push_back(retryNode);
}

void SynParser::parseTimeoutNode(){
    ASTToutNodeSPtr toutNode = std::make_shared<ASTToutNode>();
    toutNode->touttk = getTokenWithExcep(TOKEN_ENUM::TOUTTK);
    toutNode->lpartk = getTokenWithExcep(TOKEN_ENUM::LPARTK);
    parseDuration();
    toutNode->dur = std::dynamic_pointer_cast<ASTDuration>(astStacks.back());
    astStacks.pop_back();
    toutNode->rpartk = getTokenWithExcep(TOKEN_ENUM::RPARTK);
    parseNode();
    toutNode->node = std::dynamic_pointer_cast<ASTNode>(astStacks.back());
    astStacks.pop_back();
    astStacks.push_back(toutNode);
}

void SynParser::parseSkillCall(){
    ASTSkillCallSPtr skillCall = std::make_shared<ASTSkillCall>();
    skillCall->dotk = getTokenWithExcep(TOKEN_ENUM::DOTK);
    skillCall->idtk = getTokenWithExcep(TOKEN_ENUM::IDTK);
    skillCall->lpartk = getTokenWithExcep(TOKEN_ENUM::LPARTK);
    parseBindSections();
    skillCall->bindsec = std::dynamic_pointer_cast<ASTBindSec>(astStacks.back());
    astStacks.pop_back();
    skillCall->rpartk = getTokenWithExcep(TOKEN_ENUM::RPARTK);
    skillCall->semitk = getTokenWithExcep(TOKEN_ENUM::SEMITK);
    astStacks.push_back(skillCall);
}

void SynParser::parseTaskCall(){
    ASTTaskCallSPtr taskCall = std::make_shared<ASTTaskCall>();
    taskCall->calltk = getTokenWithExcep(TOKEN_ENUM::CALLTK);
    taskCall->idtk = getTokenWithExcep(TOKEN_ENUM::IDTK);
    taskCall->lpartk = getTokenWithExcep(TOKEN_ENUM::LPARTK);
    parseBindSections();
    taskCall->bindsec = std::dynamic_pointer_cast<ASTBindSec>(astStacks.back());
    astStacks.pop_back();
    taskCall->rpartk = getTokenWithExcep(TOKEN_ENUM::RPARTK);
    taskCall->semitk = getTokenWithExcep(TOKEN_ENUM::SEMITK);
    astStacks.push_back(taskCall);
}  


void SynParser::parseCheckStmt(){
    ASTCheckStmtSPtr checkStmt = std::make_shared<ASTCheckStmt>();
    checkStmt->ccktk = getTokenWithExcep(TOKEN_ENUM::CCKTK);
    checkStmt->lpartk = getTokenWithExcep(TOKEN_ENUM::LPARTK);
    parseCondExp();
    checkStmt->condExp = std::dynamic_pointer_cast<ASTCondExp>(astStacks.back());
    astStacks.pop_back();
    checkStmt->rpartk = getTokenWithExcep(TOKEN_ENUM::RPARTK);
    checkStmt->semitk = getTokenWithExcep(TOKEN_ENUM::SEMITK);
    astStacks.push_back(checkStmt);
}

void SynParser::parseWaitStmt(){
    ASTWaitStmtSPtr waitStmt = std::make_shared<ASTWaitStmt>();
    waitStmt->waittk = getTokenWithExcep(TOKEN_ENUM::WAITTK);
    waitStmt->lpartk = getTokenWithExcep(TOKEN_ENUM::LPARTK);
    parseDuration();
    waitStmt->dur = std::dynamic_pointer_cast<ASTDuration>(astStacks.back());
    astStacks.pop_back();
    waitStmt->rpartk = getTokenWithExcep(TOKEN_ENUM::RPARTK);
    waitStmt->semitk = getTokenWithExcep(TOKEN_ENUM::SEMITK);
    astStacks.push_back(waitStmt);
}

void SynParser::parseBindSections(){
    ASTBindSecSPtr bindSec = std::make_shared<ASTBindSec>();
    if(peekToken() == TOKEN_ENUM::IDTK){
        parseInBindings();
        bindSec->inbinds = std::dynamic_pointer_cast<ASTInBindings>(astStacks.back());
        astStacks.pop_back();
    }
    bindSec->semitk = getTokenWithExcep(TOKEN_ENUM::SEMITK);
    if(peekToken() == TOKEN_ENUM::IDTK){
        parseOutBindings();
        bindSec->outbinds = std::dynamic_pointer_cast<ASTOutBindings>(astStacks.back());
        astStacks.pop_back();
    }
    astStacks.push_back(bindSec);
}

void SynParser::parseInBindings(){
    ASTInBindingsSPtr inbs = std::make_shared<ASTInBindings>();
    parseInBinding();
    inbs->inbs.push_back(std::dynamic_pointer_cast<ASTInBinding>(astStacks.back()));
    astStacks.pop_back();
    TOKEN_ENUM te = peekToken();
    while(te == TOKEN_ENUM::COMTK){
        inbs->commatks.push_back(getTokenWithExcep(te));
        parseInBinding();
        inbs->inbs.push_back(std::dynamic_pointer_cast<ASTInBinding>(astStacks.back()));
        astStacks.pop_back();
        te = peekToken();
    }
    astStacks.push_back(inbs);
}

void SynParser::parseOutBindings(){
    ASTOutBindingsSPtr outbs = std::make_shared<ASTOutBindings>();
    parseOutBinding();
    outbs->outbs.push_back(std::dynamic_pointer_cast<ASTOutBinding>(astStacks.back()));
    astStacks.pop_back();
    TOKEN_ENUM te = peekToken();
    while(te == TOKEN_ENUM::COMTK){
        outbs->commatks.push_back(getTokenWithExcep(te));
        parseOutBinding();
        outbs->outbs.push_back(std::dynamic_pointer_cast<ASTOutBinding>(astStacks.back()));
        astStacks.pop_back();
        te = peekToken();
    }
    astStacks.push_back(outbs);
}

void SynParser::parseInBinding(){
    ASTInBindingSPtr inb = std::make_shared<ASTInBinding>();
    inb->idtk = getTokenWithExcep(TOKEN_ENUM::IDTK);
    inb->asgtk = getTokenWithExcep(TOKEN_ENUM::ASGNTK);
    parseValue();
    inb->value = std::dynamic_pointer_cast<ASTValue>(astStacks.back());
    astStacks.pop_back();
    astStacks.push_back(inb);
}

void SynParser::parseOutBinding(){
    ASTOutBindingSPtr outb = std::make_shared<ASTOutBinding>();
    outb->idtk = getTokenWithExcep(TOKEN_ENUM::IDTK);
    outb->arrtk = getTokenWithExcep(TOKEN_ENUM::ARWTK);
    parseRef();
    outb->ref = std::dynamic_pointer_cast<ASTRef>(astStacks.back());
    astStacks.pop_back();
    astStacks.push_back(outb);
}

void SynParser::parseValue(){
    ASTValueSPtr value = std::make_shared<ASTValue>();
    TOKEN_ENUM te = peekToken();
    if(te == TOKEN_ENUM::IDTK){
        parseRef();
        value->ref = std::dynamic_pointer_cast<ASTRef>(astStacks.back());
        astStacks.pop_back();
    } else {
        parseLiteral();
        value->literal = std::dynamic_pointer_cast<ASTLiteral>(astStacks.back());
        astStacks.pop_back();
    }
    astStacks.push_back(value);
}

void SynParser::parseCondExp(){
    ASTCondExpSPtr condExp = std::make_shared<ASTCondExp>();
    parseLOrExp();
    condExp->lorExp = std::dynamic_pointer_cast<ASTLOrExp>(astStacks.back());
    astStacks.pop_back();
    astStacks.push_back(condExp);
}

void SynParser::parseLOrExp(){
    ASTLOrExpSPtr lorExp = std::make_shared<ASTLOrExp>();
    parseLAndExp();
    lorExp->landExps.push_back(std::dynamic_pointer_cast<ASTLAndExp>(astStacks.back()));
    astStacks.pop_back();
    TOKEN_ENUM te = peekToken();
    while(te == TOKEN_ENUM::ORTK){
        lorExp->ortks.push_back(getTokenWithExcep(te));
        parseLAndExp();
        lorExp->landExps.push_back(std::dynamic_pointer_cast<ASTLAndExp>(astStacks.back()));
        astStacks.pop_back();
        te = peekToken();
    }
    astStacks.push_back(lorExp);
}

void SynParser::parseLAndExp(){
    ASTLAndExpSPtr landExp = std::make_shared<ASTLAndExp>();
    parseEqExp();
    landExp->eqExps.push_back(std::dynamic_pointer_cast<ASTEqExp>(astStacks.back()));
    astStacks.pop_back();
    TOKEN_ENUM te = peekToken();
    while(te == TOKEN_ENUM::ANDTK){
        landExp->andtks.push_back(getTokenWithExcep(te));
        parseEqExp();
        landExp->eqExps.push_back(std::dynamic_pointer_cast<ASTEqExp>(astStacks.back()));
        astStacks.pop_back();
        te = peekToken();
    }
    astStacks.push_back(landExp);
}

void SynParser::parseEqExp(){
    ASTEqExpSPtr eqExp = std::make_shared<ASTEqExp>();
    parseRelExp();
    eqExp->relExps.push_back(std::dynamic_pointer_cast<ASTRelExp>(astStacks.back()));
    astStacks.pop_back();
    TOKEN_ENUM te = peekToken();
    while(te == TOKEN_ENUM::EQLTK || te == TOKEN_ENUM::NEQTK){
        eqExp->optks.push_back(getTokenWithExcep(te));
        parseRelExp();
        eqExp->relExps.push_back(std::dynamic_pointer_cast<ASTRelExp>(astStacks.back()));
        astStacks.pop_back();
        te = peekToken();
    }
    astStacks.push_back(eqExp);
}

void SynParser::parseRelExp(){
    ASTRelExpSPtr relExp = std::make_shared<ASTRelExp>();
    parseAddExp();
    relExp->addExp1 = std::dynamic_pointer_cast<ASTAddExp>(astStacks.back());
    astStacks.pop_back();
    TOKEN_ENUM te = peekToken();
    if(te == TOKEN_ENUM::GEQTK || te == TOKEN_ENUM::GRTTK || te == TOKEN_ENUM::LEQTK || te == TOKEN_ENUM::LSSTK){
        relExp->ops = getTokenWithExcep(te);
        parseAddExp();
        relExp->addExp2 = std::dynamic_pointer_cast<ASTAddExp>(astStacks.back());
        astStacks.pop_back();
    }
    astStacks.push_back(relExp);
}

void SynParser::parseAddExp(){
    ASTAddExpSPtr addExp = std::make_shared<ASTAddExp>();
    parseMulExp();
    addExp->mulExps.push_back(std::dynamic_pointer_cast<ASTMulExp>(astStacks.back()));
    astStacks.pop_back();
    TOKEN_ENUM te = peekToken();
    while(te == TOKEN_ENUM::PLUSTK || te == TOKEN_ENUM::MINUSTK){
        addExp->optks.push_back(getTokenWithExcep(te));
        parseMulExp();
        addExp->mulExps.push_back(std::dynamic_pointer_cast<ASTMulExp>(astStacks.back()));
        astStacks.pop_back();
        te = peekToken();
    }
    astStacks.push_back(addExp);
}

void SynParser::parseMulExp(){
    ASTMulExpSPtr mulExp = std::make_shared<ASTMulExp>();
    parseUnaryExp();
    mulExp->unaryExps.push_back(std::dynamic_pointer_cast<ASTUnaryExp>(astStacks.back()));
    astStacks.pop_back();
    TOKEN_ENUM te = peekToken();
    while(te == TOKEN_ENUM::MULTK || te == TOKEN_ENUM::DIVTK || te == TOKEN_ENUM::MODTK){
        mulExp->optks.push_back(getTokenWithExcep(te));
        parseUnaryExp();
        mulExp->unaryExps.push_back(std::dynamic_pointer_cast<ASTUnaryExp>(astStacks.back()));
        astStacks.pop_back();
        te = peekToken();
    }
    astStacks.push_back(mulExp);
}

void SynParser::parseUnaryExp(){
    ASTUnaryExpSPtr unaryExp = std::make_shared<ASTUnaryExp>();
    TOKEN_ENUM te = peekToken();
    if(te == TOKEN_ENUM::PLUSTK || te == TOKEN_ENUM::NOTTK){
        unaryExp->optk = getTokenWithExcep(te);
        parseUnaryExp();
        unaryExp->unaryExp = std::dynamic_pointer_cast<ASTUnaryExp>(astStacks.back());
        astStacks.pop_back();
    } else{
        parsePrimaryExp();
        unaryExp->primaryExp = std::dynamic_pointer_cast<ASTPrimaryExp>(astStacks.back());
        astStacks.pop_back();
    }
    astStacks.push_back(unaryExp);
}

void SynParser::parsePrimaryExp(){
    ASTPrimaryExpSPtr priExp = std::make_shared<ASTPrimaryExp>();
    TOKEN_ENUM te = peekToken();
    if(te == TOKEN_ENUM::IDTK){
        parseRef();
        priExp->ref = std::dynamic_pointer_cast<ASTRef>(astStacks.back());
        astStacks.pop_back();
    } else if(te == TOKEN_ENUM::LPARTK){
        priExp->lpartk = getTokenWithExcep(TOKEN_ENUM::LPARTK);
        parseCondExp();
        priExp->condExp = std::dynamic_pointer_cast<ASTCondExp>(astStacks.back());
        astStacks.pop_back();
        priExp->rpartk = getTokenWithExcep(TOKEN_ENUM::RPARTK);
    } else {
        parseLiteral();
        priExp->literal = std::dynamic_pointer_cast<ASTLiteral>(astStacks.back());
        astStacks.pop_back();
    }
    astStacks.push_back(priExp);
}

void SynParser::parseLiteral(){
    ASTLiteralSPtr literal = std::make_shared<ASTLiteral>();
    TOKEN_ENUM te = peekToken();
    TokenSPtr tk = getTokenWithExcep(te);
    if(te == TOKEN_ENUM::LITK){
        literal->ltype = ASTLiteral::AST_LITERAL_TYPE::INT;
        literal->val = tk->getNumValue();
    } else if(te == TOKEN_ENUM::LFTK){
        literal->ltype = ASTLiteral::AST_LITERAL_TYPE::DOUBLE;
        literal->val = tk->getNumValue();
    } else if(te == TOKEN_ENUM::LBTK){
        literal->ltype = ASTLiteral::AST_LITERAL_TYPE::BOOL;
        literal->val = tk->getNumValue();
    } else if(te == TOKEN_ENUM::LSTK){
        literal->ltype = ASTLiteral::AST_LITERAL_TYPE::STRING;
        literal->content = tk->getContent();
    }
    astStacks.push_back(literal);
}

void SynParser::parseRef(){
    ASTRefSPtr ref = std::make_shared<ASTRef>();
    ref->idtk = getTokenWithExcep(TOKEN_ENUM::IDTK);
    astStacks.push_back(ref);
}

void SynParser::parseDuration(){
    ASTDurationSPtr dur = std::make_shared<ASTDuration>();
    TokenSPtr token = getTokenWithExcep(TOKEN_ENUM::LITK);
    dur->val = token->getNumValue();
    astStacks.push_back(dur);
}

void SynParser::parseType(){
    ASTTypeSPtr type = std::make_shared<ASTType>();
    TOKEN_ENUM tk = peekToken();
    if(tk == TOKEN_ENUM::IDTK){
        TokenSPtr token = getTokenWithExcep(TOKEN_ENUM::IDTK);
        type->selfDefType = token;
    } else {
        parseBuiltinType();
        type->buildinType = std::dynamic_pointer_cast<ASTBuiltinType>(astStacks.back());   
        astStacks.pop_back(); 
    }
    astStacks.push_back(type);
}

void SynParser::parseBuiltinType(){
    ASTBuiltinTypeSPtr btype = std::make_shared<ASTBuiltinType>();
    if((btype->booltk = getTokenWithoutExcep(TOKEN_ENUM::BOOLTK))) ;
    else if(btype->doubletk = getTokenWithoutExcep(TOKEN_ENUM::DOUBLETK)) ;
    else if(btype->inttk = getTokenWithoutExcep(TOKEN_ENUM::INTTK)) ;
    else if(btype->stringtk = getTokenWithoutExcep(TOKEN_ENUM::STRTK));
    else throw std::runtime_error("Syntactic Error: can't parse BuildinType.");
    astStacks.push_back(btype); 
}

ASTCompUnitSPtr SynParser::parse(const std::vector<TokenSPtr>& tks){
    this->tks = tks;
    this->index = 0;
    this->astStacks.clear();
    parseCompUnit();
    ASTCompUnitSPtr ret = std::dynamic_pointer_cast<ASTCompUnit>(astStacks.back());
    astStacks.clear();
    return ret;
}