/*
 * Group no. 50
 * Aniruddha Mahajan -------- 2017A7PS0145P
 * Ravindra Singh Shekhawat - 2017A7PS0146P
 * Shreyas Srikrishna ------- 2017A7PS0162P
 * Chetan Sharma ------------ 2017A7PS0182P
 */

#include "typeExtractor.h"
#include "lexer.h"
#include "parser.h"
#include "ast.h"

#define sc startChild
#define rs rightSibling

//tells the type of the identifier being declared. might be int/real/boolean/array
//param : node : ASTNode of type declareNode
Typeof extractTypeOfId(ASTNode* node){
    
    ASTNode *temp = node->sc->rs;   
    if(temp->type == arrayIdNode)
    {
        if(temp->sc->sc->type == idNode || temp->sc->sc->rs->type == idNode)
        {
            temp = temp->sc->rs;
            if(strcmp(temp->node.typeNode.token,"INTEGER"))
            {
                node->node.declareNode.typeOfId = dynamicArrayInt;
                return dynamicArrayInt;
            }
            else if(strcmp(temp->node.typeNode.token,"REAL"))
            {
                node->node.declareNode.typeOfId = dynamicArrayReal;
                return dynamicArrayReal;
            }
            else if(strcmp(temp->node.typeNode.token,"BOOLEAN"))
            {
                node->node.declareNode.typeOfId = dynamicArrayBool;
                return dynamicArrayBool;
            }
            else
            {
                return -1;        
            }
        }           
        else
        {
            temp = temp->sc->rs;
            if(strcmp(temp->node.typeNode.token,"INTEGER"))
            {
                return staticArrayInt;
            }
            else if(strcmp(temp->node.typeNode.token,"REAL"))
            {
                return staticArrayReal;
            }
            else if(strcmp(temp->node.typeNode.token,"BOOLEAN"))
            {
                return staticArrayBool;
            }
            else
            {
                return -1;        
            }
        }
       
    }
    else if(temp->type == typeNode)
    {
        if(strcmp(temp->node.typeNode.token,"INTEGER"))
        {
            return integer;
        }
        else if(strcmp(temp->node.typeNode.token,"REAL"))
        {
            return real;
        }
        else if(strcmp(temp->node.typeNode.token,"BOOLEAN"))
        {
            return boolean;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return -1;            
    }
    return -1;
}

//tells the type of a function(input/output parameter types)
//note: function cannot return array
//param : node : ASTNode of type moduleNode
FunctionType* extractTypeOfFunction(ASTNode* node){
    ASTNode* inputs = node->sc->rs;         //head of inputParamNodes list
    ASTNode* outputs = node->sc->rs->rs;    //head of outputParamNodes list. might be nullNode
    FunctionType* functionType = (FunctionType*)malloc(sizeof(FunctionType));
    
    //compute number of input parameters
    int no_of_inputs=0;
    while(inputs != NULL){
        no_of_inputs++;
        inputs = inputs->next;
    }
    inputs = node->sc->rs;
    
    //compute number of output parameters
    int no_of_outputs=0;
    if(outputs->type != nullNode){
        while(outputs != NULL){
            no_of_outputs++;
            outputs = outputs->next;
        }
    }
    outputs = node->sc->rs->rs;

    Typeof* inputType = (Typeof*)malloc(sizeof(Typeof)*no_of_inputs);
    Typeof* outputType = (Typeof*)malloc(sizeof(Typeof)*no_of_outputs);

    int i;
    //populate inputType array
    for(i=0; i<no_of_inputs; i++){
        if(inputs->sc->rs->type == typeNode){
            if(strcmp(inputs->sc->rs->node.typeNode.token, "BOOLEAN"))
                inputType[i] = boolean;
            else if(strcmp(outputs->sc->rs->node.typeNode.token, "REAL"))
                outputType[i] = real;
            else if(strcmp(outputs->sc->rs->node.typeNode.token, "INTEGER"))
                outputType[i] = integer;
            else
                return NULL;
        }
        else if(inputs->sc->rs->type == arrayTypeNode){
            if(strcmp(inputs->sc->rs->sc->rs->node.typeNode.token, "BOOLEAN")){
                if(inputs->sc->rs->sc->sc->type == numNode && inputs->sc->rs->sc->sc->rs->type == numNode)
                    inputType[i] = staticArrayBool;
                else
                    outputType[i] = dynamicArrayBool;
            }   
            else if(strcmp(inputs->sc->rs->sc->rs->node.typeNode.token, "REAL")){
                if(inputs->sc->rs->sc->sc->type == numNode && inputs->sc->rs->sc->sc->rs->type == numNode)
                    inputType[i] = staticArrayReal;
                else
                    outputType[i] = dynamicArrayReal;
            } 
            else if(strcmp(inputs->sc->rs->sc->rs->node.typeNode.token, "INTEGER")){
                if(inputs->sc->rs->sc->sc->type == numNode && inputs->sc->rs->sc->sc->rs->type == numNode)
                    inputType[i] = staticArrayInt;
                else
                    outputType[i] = dynamicArrayInt;
            }
            else
                return NULL;  
        }
        else
            return NULL;
        inputs = inputs->next;
    }

    //populate outputType array
    for(i=0; i<no_of_outputs; i++){
        if(strcmp(outputs->sc->rs->node.typeNode.token, "BOOLEAN"))
            outputType[i] = boolean;
        else if(strcmp(outputs->sc->rs->node.typeNode.token, "REAL"))
            outputType[i] = real;
        else if(strcmp(outputs->sc->rs->node.typeNode.token, "INTEGER"))
            outputType[i] = integer;
        else
            return NULL;
        outputs = outputs->next;
    }
    
    functionType->inputList = inputType;
    functionType->outputList = outputType;
    node->node.moduleNode.typeOfFunc = functionType;
    return functionType;
}


//extracts type of an expression
//param : node: ASTNode of type opNode
Typeof extractTypeOfExpression(ASTNode *node)
{
    
}