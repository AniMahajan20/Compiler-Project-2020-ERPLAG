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
        }
        case moduleNode:{
            //check if driverModule
            if(node->rs->rs == NULL && node->rs->type == moduleNode && node->parent->type == programNode){
                SymbolTableEntry* curr = lookupString("driverModule", table, functionEntry, false);
                if(curr==NULL){
                    printf("\nSymbotable populated wrongly\n");
                    //not possible
                }
                
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
        }

        case declareNode:{
            //do nothing
        }

        case opNode:{
            extractTypeOfExpression(node, table, semanticErrors);
        }

        case assignmentNode:{
            //first check if lhs variable has been declared
            
            //PrimitiveType t = extractTypeOfExpression(node->sc->rs, table, semanticErrors);

            //1. check if for loop iterating variable has been assigned 
    
            //pehle ye check karna padega ki are we inside forloop scope or not.. 
            //yes for that we need to perform lookup in symboltable..block se lookup karna padega
            //matlab for loop ke us iterating variable tak phochne ke lie us ForLoopNode(ASTNODE) ki zaroorat he, correct?

        //oooh got it
        //basically saare nested for loops check karne padenge
        // haa aur yeh chota so code kar dega woh kaam
            // sunn ek case hai jo t5 me hai yeh ek ex hai neeche wala
            // for a in 5...10
            //    for b in 100...200  
            //      switch case
            //          yahan pe a = b+c;  //
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
            }    //lol 
            //accha fir while loop ka batata hu. 
            //basically i suggest making a bool flag variable in whileLoopNode. jaise hi uske statements me
            //iterating variable ka assignment ho jaye, to flag true kar do. 
            //fir pura loop traverse karke dekhne ke bad check flag.. 
        } // sunn ek min
            // while i<=10  //yeh valid hai ??
            //   i = i+1; 
            //   i = i+2;
            //fir usne t5.txt me kyu likha he aisa
            //ruk language specifications padhte he
            //usne waise semantic rules (nalanda wala) 
//yes valid . o.n.ly  need 1 assignment statement // sun na fir toh agar nhi hoga toh infinte loop hoga woh run time issue me nhi aayega ??
        case functionCallNode:{
            ////accha ruk. me mail likhta hu, use bolta hu ki aise bohot sare cheeze he check karneko
            //kya aap rules ki list upload karoge ya nahi   //ok wait
            // hmm yes that should work
        }
    }
}
// a = b*c

//declare b;