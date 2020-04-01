/*
 * Group no. 50
 * Aniruddha Mahajan -------- 2017A7PS0145P
 * Ravindra Singh Shekhawat - 2017A7PS0146P
 * Shreyas Srikrishna ------- 2017A7PS0162P
 * Chetan Sharma ------------ 2017A7PS0182P
 */

#include "typeExtractor.h"
#include "semanticDef.h"
#include "astDef.h"
#include <string.h>

#define sc startChild
#define rs rightSibling

//returns pointer to object of type Typeof
//NOTE: pass l and h as -1,-1 in case you need a primitive object
Typeof* createTypeof(VariableTypeTag tag1, PrimitiveType tag2, int l, int h){
    if(tag1 == primitive){
        Typeof* t = (Typeof*)malloc(sizeof(Typeof));
        t->tag = tag1;
        t->type.primitiveType = tag2;
        return t;
    }
    else if(tag1 == array){
        Typeof* t = (Typeof*)malloc(sizeof(Typeof));
        t->tag = tag1;
        t->type.arrayType.high = h;
        t->type.arrayType.low = l;
        return t;
    }
    else{
        return NULL;
    }
    
}

//tells the type of the identifier being declared. might be int/real/boolean/array
//param : node : ASTNode of type declareNode
Typeof* extractTypeOfId(ASTNode* node){
    
    ASTNode *temp = node->sc->rs;   
    if(temp->type == arrayIdNode)
    {
        if(temp->sc->sc->type == idNode || temp->sc->sc->rs->type == idNode)
        {
            
            temp = temp->sc->rs;
            if(strcmp(temp->node.typeNode.token,"INTEGER")==0)
            {
                
                //node->node.declareNode.typeOf = 
                return dynamicArrayInt;
            }
            else if(strcmp(temp->node.typeNode.token,"REAL")==0)
            {
                node->node.declareNode.typeOfId = dynamicArrayReal;
                return dynamicArrayReal;
            }
            else if(strcmp(temp->node.typeNode.token,"BOOLEAN")==0)
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
            if(strcmp(temp->node.typeNode.token,"INTEGER")==0)
            {
                node->node.declareNode.typeOfId = staticArrayInt;
                return staticArrayInt;
            }
            else if(strcmp(temp->node.typeNode.token,"REAL")==0)
            {
                node->node.declareNode.typeOfId = staticArrayReal;
                return staticArrayReal;
            }
            else if(strcmp(temp->node.typeNode.token,"BOOLEAN")==0)
            {
                node->node.declareNode.typeOfId = staticArrayBool;
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
        if(strcmp(temp->node.typeNode.token,"INTEGER")==0)
        {
            Typeof* typeofnode = createTypeof(primitive,integer,-1,-1);
            node->node.declareNode.typeOfId = *typeofnode;
            return integer;
        }
        else if(strcmp(temp->node.typeNode.token,"REAL")==0)
        {
            Typeof* typeofnode = createTypeof(primitive,real,-1,-1);
            node->node.declareNode.typeOfId = *typeofnode;
            return real;
        }
        else if(strcmp(temp->node.typeNode.token,"BOOLEAN")==0)
        {
            Typeof* typeofnode = createTypeof(primitive,boolean,-1,-1);
            node->node.declareNode.typeOfId = *typeofnode;
            return typeofnode;
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
            if(strcmp(inputs->sc->rs->node.typeNode.token, "BOOLEAN")==0)
                inputType[i] = *createTypeof(primitive, boolean, -1, -1);
            else if(strcmp(outputs->sc->rs->node.typeNode.token, "REAL")==0)
                inputType[i] = *createTypeof(primitive, real, -1, -1);
            else if(strcmp(outputs->sc->rs->node.typeNode.token, "INTEGER")==0)
                inputType[i] = *createTypeof(primitive, integer, -1, -1);
            else
                return NULL;
        }
        else if(inputs->sc->rs->type == arrayTypeNode){
            int lower = inputs->sc->rs->sc->sc->node.numNode.value;
            int higher = inputs->sc->rs->sc->sc->rs->node.numNode.value;
            if(strcmp(inputs->sc->rs->sc->rs->node.typeNode.token, "BOOLEAN")==0){
                if(inputs->sc->rs->sc->sc->type == numNode && inputs->sc->rs->sc->sc->rs->type == numNode)
                    inputType[i] = *createTypeof(array, boolean, lower, higher);
                else if(inputs->sc->rs->sc->sc->type == numNode && inputs->sc->rs->sc->sc->rs->type == idNode)
                    inputType[i] = *createTypeof(array, boolean, lower, -1);
                else if(inputs->sc->rs->sc->sc->type == idNode && inputs->sc->rs->sc->sc->rs->type == numNode)
                    inputType[i] = *createTypeof(array, boolean, lower, -1);
                }
            }   
            else if(strcmp(inputs->sc->rs->sc->rs->node.typeNode.token, "REAL")==0){
                if(inputs->sc->rs->sc->sc->type == numNode && inputs->sc->rs->sc->sc->rs->type == numNode){
                    inputType[i] = staticArrayReal;
                }
                else{
                    outputType[i] = dynamicArrayReal;
                }
            } 
            else if(strcmp(inputs->sc->rs->sc->rs->node.typeNode.token, "INTEGER")==0){
                if(inputs->sc->rs->sc->sc->type == numNode && inputs->sc->rs->sc->sc->rs->type == numNode){
                    inputType[i] = staticArrayInt;
                }
                else{
                    outputType[i] = dynamicArrayInt;
                }
            else if(strcmp(inputs->sc->rs->sc->rs->node.typeNode.token, "INTEGER")==0){
                if(inputs->sc->rs->sc->sc->type == numNode && inputs->sc->rs->sc->sc->rs->type == numNode){
                    inputType[i] = staticArrayInt;
                }
                else{
                    outputType[i] = dynamicArrayInt;
                }
            else if(strcmp(inputs->sc->rs->sc->rs->node.typeNode.token, "INTEGER")==0){
                if(inputs->sc->rs->sc->sc->type == numNode && inputs->sc->rs->sc->sc->rs->type == numNode){
                    inputType[i] = staticArrayInt;
                }
                else{
                    outputType[i] = dynamicArrayInt;
                }
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
        if(strcmp(outputs->sc->rs->node.typeNode.token, "BOOLEAN")==0)
            outputType[i] = boolean;
        else if(strcmp(outputs->sc->rs->node.typeNode.token, "REAL")==0)
            outputType[i] = real;
        else if(strcmp(outputs->sc->rs->node.typeNode.token, "INTEGER")==0)
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

//extracts type of an expression, and also sets appropriate field in the AST's opNode
//param : node: ASTNode of type opNode
Typeof extractTypeOfExpression(ASTNode *node)
{
    if(strcmp(node->node.opNode.token,"PLUS")==0){
        
    }
    else if(strcmp(node->node.opNode.token,"MINUS")==0){

    }
    else if(strcmp(node->node.opNode.token,"MUL")==0){ 

    }
    else if(strcmp(node->node.opNode.token,"DIV")==0){

    }
    else if(strcmp(node->node.opNode.token, "LT")==0){  

    }
    else if(strcmp(node->node.opNode.token, "LE")==0){

    }
    else if(strcmp(node->node.opNode.token, "GT")==0){

    }
    else if(strcmp(node->node.opNode.token, "GE")==0){

    }
    else if(strcmp(node->node.opNode.token, "EQ")==0){

    }
    else if(strcmp(node->node.opNode.token, "NE")==0){

    }
    else if(strcmp(node->node.opNode.token,"AND")==0){

    }
    else if(strcmp(node->node.opNode.token, "OR")==0){

    }
    else{
        //node is not opnode
    }
}