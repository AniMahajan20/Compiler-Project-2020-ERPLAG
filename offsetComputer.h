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

void computeOffsets(ASTNode* root, SymbolTable* rootSymbolTable);
void calcOffsets(ASTNode* node,SymbolTable* rootSymbolTable);
void processStatement(ASTNode* node, SymbolTable* table);
void processTemporaries(ASTNode* currMod, int currOffset, SymbolTable* rootSymbolTable);
void printSymbolTable(SymbolTable* root);
void printArrays(SymbolTable* root);
void printActivationRecord(SymbolTable* root);