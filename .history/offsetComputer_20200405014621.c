/*
 * Group no. 50
 * Aniruddha Mahajan -------- 2017A7PS0145P
 * Ravindra Singh Shekhawat - 2017A7PS0146P
 * Shreyas Srikrishna ------- 2017A7PS0162P
 * Chetan Sharma ------------ 2017A7PS0182P
 */


#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "symboltable.h"
#include "typeExtractor.h"
#include "semantic.h"
#include "functionCheck.h"
#include "offsetComputer.h"
#include <string.h>
#define sc startChild
#define rs rightSibling

int offset;

void computeOffsets(ASTNode* root, SymbolTable* rootSymbolTable){
    ASTNode* otherMod = root->sc->rs;  
    ASTNode* driverMod = root->sc->rs->rs;
    ASTNode* otherMod2 = root->sc->rs->rs->rs;
    while(otherMod != NULL){
        offset = 0;
        calcOffsets(otherMod, rootSymbolTable);
        otherMod = otherMod->next;
    }
    offset = 0;
    calcOffsets(driverMod, rootSymbolTable);   
    while(otherMod2 != NULL){
        offset = 0;
        calcOffsets(otherMod2, rootSymbolTable);
        otherMod2 = otherMod->next;
    }
}
    

void calcOffsets(ASTNode* currModule, SymbolTable* rootSymbolTable){
    // yeh mai likh deta hoon okk??
    // ek ar cheeze yeh function ek module ke andr offset set karra hai kya??
   //yesthis function is for current module only.. cu
   //just traverse it's statemments
   //ruk
   ASTNode* stmtNode = currModule->sc->rs->rs->rs;
   SymbolTableEntry* sym = lookupString(currModule->sc->node.idnode.lexeme,rootSymbolTable,functionEntry,false);
   SymbolTable* currTable = sym->table;//ye calcculate karke neeche function me pass karna he
   while(stmtNode != NULL){
       processStmt(stmtNode, currTable);
       stmtNode = stmtNode->next;
   } 
}

void processStmt(ASTNode* stmtNode, SymbolTable* currTable){
    switch(stmtNode->type){
        case declareNode:{
            Typeof* declareType = &stmtNode->node.declareNode.typeOfId;
            if(declareType->tag == array){
                if(declareType->type.arrayType.high>=0 && declareType->type.arrayType.low>=0){  //static
                    int factor;
                    switch(declareType->type.arrayType.t){
                        case integer: {factor=2; break;}
                        case real: {factor=4; break;}
                        case boolean: {factor=1; break;}
                    }
                    int currWidth = factor*(declareType->type.arrayType.high - declareType->type.arrayType.low + 1);
                    ASTNode* declareId = stmtNode->sc;
                    while(declareId != NULL){
                        SymbolTableEntry* sym = lookupString(declareId->node.idnode.lexeme,currTable,idEntry,false);
                        sym->symbol.idEntry.offset = offset;
                        offset += currWidth;
                        declareId = declareId->next;
                    }
                }
                else{       //dynamic..assign width 2
                    ASTNode* declareId = stmtNode->sc;
                    while(declareId != NULL){
                        SymbolTableEntry* sym = lookupString(declareId->node.idnode.lexeme,currTable,idEntry,false);
                        sym->symbol.idEntry.offset = offset;
                        offset += 2;
                        declareId = declareId
                    }
                }
            }
            else{       //primitive
                ASTNode* idtraverse = stmtNode->sc;
                while(idtraverse != NULL)
                {
                    SymbolTableEntry* sym = lookupString(idtraverse->node.idnode.lexeme,currTable,idEntry,false);
                    PrimitiveType t = sym->symbol.idEntry.type.type.primitiveType;
                    if(t == integer){
                        sym->symbol.idEntry.offset = offset;
                        offset += 2;
                    }
                    else if(t == boolean){
                        sym->symbol.idEntry.offset = offset;
                        offset += 1;
                    }
                    else { // real  
                        sym->symbol.idEntry.offset = offset;
                        offset += 4;
                    }
                    idtraverse = idtraverse->next; 
                }
            }
            break;
        }
        case forLoopNode:{
            ASTNode* forLoopStmt = stmtNode->sc->rs->rs;
            SymbolTableEntry* sym = lookupBlock(&stmtNode->node.forLoopNode.block,currTable,forLoopEntry,false);
            SymbolTable* curr= sym->table;
            while(forLoopStmt != NULL){
                processStmt(forLoopStmt,curr);
                forLoopStmt = forLoopStmt->next;
            }
            break;
        }
        case whileLoopNode:{
            ASTNode *whileStmt = stmtNode->sc->rs;
            SymbolTableEntry* sym = lookupBlock(&stmtNode->node.whileLoopNode.block,currTable,whileLoopEntry,false);
            SymbolTable* curr= sym->table;
            //(Block* b, SymbolTable* table, SymbolForm f, bool deepSearch);
            while(whileStmt != NULL)
            {
                processStmt(whileStmt,curr);
                whileStmt = whileStmt->next;
            }
            break;
        }
        case conditionalNode:{
            SymbolTableEntry* sym = lookupBlock(&stmtNode->node.conditionalNode.block,currTable,whileLoopEntry,false);
            SymbolTable* curr = sym->table;
            ASTNode* caseStmt = stmtNode->sc->rs;
            while(caseStmt != NULL){
                processStmt(caseStmt, curr);
                caseStmt = caseStmt->next;
            }
            if(stmtNode->sc->rs->rs->type != nullNode)
                processStmt(stmtNode->sc->rs->rs, curr);
            break;
        }
        case caseNode:{
            ASTNode* stmt = stmtNode->sc;
            while(stmt != NULL){
                processStmt(stmt, currTable);
                stmt = stmt->next;
            }
        }
        default:{
            break;
        }
    }
}
