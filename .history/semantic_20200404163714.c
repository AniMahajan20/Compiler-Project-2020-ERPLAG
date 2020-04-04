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
#include <string.h>
#define sc startChild
#define rs rightSibling

ListOfErrors* semanticErrors;
// kya likh rha hai??
//this is semantic.c
//we will initiate symboltable construction from this file only. yahan se processAST() call karenge??

//
ListOfErrors* getSemanticErrorObject()
{
    return semanticErrors;
}

//then call the typechecking functions from here
//for the same, we will pass error object to every function
void initializeErrorObject(){
    semanticErrors = (ListOfErrors*)malloc(sizeof(ListOfErrors));
    semanticErrors->numberOfErr = 0;
}

Error* createErrorObject()
{
    Error *error = (Error *)malloc(sizeof(Error));
    error->next = NULL;
    return error;
}

/*Control function for semantic analysis. 
This function calls for construction of symbol table, type checking of expressions
type checking for function calls etc*/
void semanticAnalyzer(){
    initializeErrorObject();
    ASTNode* astRoot = getAST();
    
    //construct symboltable
    processAST(astRoot,NULL,semanticErrors);
    /*NOTE:In above function, following semantic checks have been performed arleady*/
    /**
     * Output parameter overloading inside module definition
     * Duplicate module Declarations
     * Duplicate variable declarations inside a block (doubt--is a variable visible inside inner scopes? can't we overshadow it? if we can, then change code in processAST where deep lookup is performed for checking duplicate declaration)
     * */

    SymbolTable* rootSymbolTable = getsymbolTable();
    //now perform semantic analysis
    analyzeAST(astRoot, rootSymbolTable, semanticErrors);
}

/*This recursive function traverses AST and performs various semantic checks*/
void analyzeAST(ASTNode* node, SymbolTable* table, ListOfErrors* semanticErrors){
    switch(node->type){
        case programNode:{
            analyzeAST(node->sc->rs, table, semanticErrors);
            analyzeAST(node->sc->rs->rs, table, semanticErrors);
            analyzeAST(node->sc->rs->rs->rs, table, semanticErrors);
            break;
        }
        case moduleNode:{
            //check if driverModule
            if(node->rs->rs == NULL && node->rs->type == moduleNode && node->parent->type == programNode){
                SymbolTableEntry* curr = lookupString("driverModule", table, functionEntry, false);
                if(curr==NULL)       //not possible
                    printf("\nSymbotable populated wrongly\n");
                
                //now process its statements
                //curr->table gives us the table for this scope
                ASTNode* traverse = node->sc->rs->rs->rs;
                while(traverse != NULL)
                    analyzeAST(traverse, curr->table, semanticErrors);
            }

            //if it's not a driver module
            else{   
                //first obtain the symboltable corresponding to this module
                SymbolTableEntry* curr = lookupString(node->sc->node.idnode.lexeme, table, functionEntry, false);
                if(curr == NULL){
                    printf("\nRoot Symboltable not populated with this function");
                    //not possible. check code
                }
                
                //now process its statements
                //curr->table gives us the table for this scope
                ASTNode* traverse = node->sc->rs->rs->rs;
                while(traverse != NULL)
                    analyzeAST(traverse, curr->table, semanticErrors);
            }
            break;
        }

        case declareNode:{
            break;
        }

        case opNode:{
            extractTypeOfExpression(node, table, semanticErrors);
            break;
        }

        case assignmentNode:{
            //first check if lhs variable has been declared

            //1. check if 'for-loop' iterating variable has been assigned 
            ASTNode *temp = node->parent;

            while(temp->parent != NULL){
                if(temp->type == forLoopNode && strcmp(temp->sc->node.idnode.lexeme,node->sc->node.idnode.lexeme)==0){
                    //semantic error
                    Error *err = createErrorObject();   err->lineNo = node->sc->node.idnode.line_no;  strcpy(err->error,"\nAssigning a value to for loop iterating variable is not allowed:"); 
                    Error *temporary = semanticErrors->head;
                    while(temporary->next != NULL)
                        temporary = temporary->next;
                    temporary->next = err; semanticErrors->numberOfErr += 1;
                    break;
                }
                else
                    temp = temp->parent;
            }

            break; 
        } 
        case functionCallNode:{
            SymbolTableEntry* func = lookupString(node->sc->rs->node.idnode.lexeme, table, functionEntry, true);
            //1.void functions handling
            if(func->symbol.functionEntry.outputListHead == NULL){  //actually a void function
                if(node->sc->type != nullNode){
                    //semantic error: void function should not return anything
                    Error *err = createErrorObject();   err->lineNo = node->sc->sc->node.idnode.line_no;  strcpy(err->error,"\nVoid function must not return anything"); 
                    Error *temporary = semanticErrors->head;
                    while(temporary->next != NULL)
                        temporary = temporary->next;
                    temporary->next = err; semanticErrors->numberOfErr += 1;
                    break;
                }
            }
            else{   //check output parameters
                Typeof* travDefOut = func->symbol.functionEntry.; ///Typeof* in o/p param types linkedlist
                ASTNode* travCallOut = node->sc;    //idNode in output paramters linkedlist
                while(travCallOut != NULL && travDefOut != NULL){
                    if(travCallOut->type == nullNode){
                        //semantic error : function must return something
                        Error *err = createErrorObject();   err->lineNo = node->sc->rs->node.idnode.line_no;  strcpy(err->error,"\nVoid function must not return anything"); 
                        Error *temporary = semanticErrors->head;
                        while(temporary->next != NULL)
                            temporary = temporary->next;
                        temporary->next = err; semanticErrors->numberOfErr += 1;
                        break;
                    }
                    SymbolTableEntry* travCallOutput = lookupString(travCallOut->node.idnode.lexeme, table, idEntry, true);
                    if(travCallOutput->symbol.idEntry.type.tag == array){
                        //semantic error : function cannot return array
                        Error *err = createErrorObject();   err->lineNo = travCallOut->node.idnode.line_no;  strcpy(err->error,"\nVoid function must not return anything"); 
                        Error *temporary = semanticErrors->head;
                        while(temporary->next != NULL)
                            temporary = temporary->next;
                        temporary->next = err; semanticErrors->numberOfErr += 1;
                        travCallOut = travCallOut->next;
                        travDefOut = travDefOut->next;
                        continue;
                    }
                    if(travCallOutput->symbol.idEntry.type.type.primitiveType != travDefOut->)
                }

                if(!(travCallOut == NULL && travDefOut == NULL)){
                    //semantic error : output parameter  count mismatch
                    Error *err = createErrorObject();   err->lineNo = travCallOut->node.idnode.line_no;  strcpy(err->error,"\nVoid function must not return anything"); 
                    Error *temporary = semanticErrors->head;
                    while(temporary->next != NULL)
                        temporary = temporary->next;
                    temporary->next = err; semanticErrors->numberOfErr += 1;
                }
            }

            
            //2. types and no of input parameters of function must be same as declaration
            ASTNode *travDefIn = func->symbol.functionEntry.inputListHead;
            ASTNode *travCallIn = node->sc->rs->rs;
            while(travCallIn != NULL){
                if(travCallIn->type == nullNode){
                    // semantic error
                    Error *err = createErrorObject();   err->lineNo = node->sc->sc->node.idnode.line_no;  strcpy(err->error,"\nVoid function must not return anything"); 
                    Error *temporary = semanticErrors->head;
                    while(temporary->next != NULL)
                        temporary = temporary->next;
                    temporary->next = err; semanticErrors->numberOfErr += 1;
                    break;
                }
                else
                {
                    SymbolTableEntry *sym = lookupString(travCallIn->node.idnode.lexeme,table,idNode,true);
                    if()
                }
                
                
            }
            
            break;
        }

        case conditionalNode:{  //switch-case. assumes swtiching variable is declared and is not of type array
            SymbolTableEntry* sym = lookupString(node->sc->node.idnode.lexeme, table, idEntry, true);
            switch(sym->symbol.idEntry.type.type.primitiveType){
                case integer:{
                    if(node->sc->rs->rs->type == nullNode)
                    {
                        // semantic error default node missing  
                        Error *err = createErrorObject();   err->lineNo = node->node.conditionalNode.block.end;  strcpy(err->error,"\nDefault Statement missing line "); 
                        Error *temporary = semanticErrors->head;
                        while(temporary->next != NULL)
                            temporary = temporary->next;
                        temporary->next = err; semanticErrors->numberOfErr += 1;
                    }
                    ASTNode *temp = node->sc->rs;
                    while(temp != NULL)
                    {
                        if(temp->sc->rs->type == numNode && strcmp(node->sc->rs->node.numNode.token,"INTEGER")==0)
                            temp = temp->next;
                        else
                        {
                            //semantic error case node is not an integer
                            Error *err = createErrorObject();   err->lineNo = node->sc->rs->node.caseNode.line;  strcpy(err->error,"\nCase Node is not an integer "); 
                            Error *temporary = semanticErrors->head;
                            while(temporary->next != NULL)
                                temporary = temporary->next;
                            temporary->next = err; semanticErrors->numberOfErr += 1;
                            //break;
                        }
                    }
                    break;
                }
                case real:{
                        Error *err = createErrorObject();   err->lineNo = node->sc->node.idnode.line_no;  strcpy(err->error,"\nSwitching variable cannot be of real type :"); 
                        Error *temporary = semanticErrors->head;
                        while(temporary->next != NULL)
                            temporary = temporary->next;
                        temporary->next = err; semanticErrors->numberOfErr += 1;
                    break;
                }
                case boolean:{
                    if(node->sc->rs->rs->type != nullNode){
                        Error *err = createErrorObject();   err->lineNo = node->sc->rs->rs->node.caseNode.line;  strcpy(err->error,"\nSwitch case on boolean variable cannot have default case"); 
                        Error *temporary = semanticErrors->head;
                        while(temporary->next != NULL)
                            temporary = temporary->next;
                        temporary->next = err; semanticErrors->numberOfErr += 1;
                    }
                    if(!(node->sc->rs->sc->rs->type == boolNode && node->sc->rs->next->sc->rs->type == boolNode && node->sc->rs->next->next == NULL)){
                        Error *err = createErrorObject();   err->lineNo = node->sc->rs->node.caseNode.line;  strcpy(err->error,"\nSwitch case on boolean variable must have exactly 2 cases, true and false"); 
                        Error *temporary = semanticErrors->head;
                        while(temporary->next != NULL)
                            temporary = temporary->next;
                        temporary->next = err; semanticErrors->numberOfErr += 1;
                    } 
                }
            }
            break;
        }            
    }
}