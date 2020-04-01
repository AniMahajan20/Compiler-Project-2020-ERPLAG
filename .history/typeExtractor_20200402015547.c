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
Typeof* createTypeof(VariableTypeTag tag1, PrimitiveType tag2, int l, int h, Typeof* arr, int index){
    if(tag1 == primitive){
        if(arr != NULL){
            arr[index].tag = primitive;
            arr[index].type.primitiveType = tag2;
            return arr;
        }
        else{
            Typeof* t = (Typeof*)malloc(sizeof(Typeof));
            t->tag = primitive;
            t->type.primitiveType = tag2;
            return t;
        }
    }
    else if(tag1 == array){
        if(arr != NULL){
            arr[index].tag = array;
            arr[index].type.arrayType.high = h;
            arr[index].type.arrayType.low = l;
            arr[index].type.arrayType.t = tag2;
            return arr;
        }
        else{
            Typeof* t = (Typeof*)malloc(sizeof(Typeof));
            t->tag = array;
            t->type.arrayType.high = h;
            t->type.arrayType.low = l;
            t->type.arrayType.t = tag2;
            return t;
        }
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
        if(temp->sc->sc->type == idNode) // low id
        {
            temp = temp->sc;
            int high = temp->sc->rs->node.numNode.value;
            temp = temp->rs;
            if(strcmp(temp->node.typeNode.token,"INTEGER")==0)
            {
                Typeof* typeofnode = createTypeof(array,integer,high,-1,NULL,-1);
                node->node.declareNode.typeOfId = *typeofnode; 
                return typeofnode;
            }
            else if(strcmp(temp->node.typeNode.token,"REAL")==0)
            {
                Typeof* typeofnode = createTypeof(array,real,high,-1,NULL,-1);
                node->node.declareNode.typeOfId = *typeofnode;
                return typeofnode;
            }
            else if(strcmp(temp->node.typeNode.token,"BOOLEAN")==0)
            {
                Typeof* typeofnode = createTypeof(array,boolean,high,-1,NULL,-1);
                node->node.declareNode.typeOfId = *typeofnode;
                return typeofnode;
            }
            else
            {
                return -1;        
            }
        }
        else if(temp->sc->sc->rs->type == idNode) // high id
        {
            temp = temp->sc;
            int low = temp->sc->node.numNode.value;
            temp = temp->rs;
            if(strcmp(temp->node.typeNode.token,"INTEGER")==0)
            {
                Typeof* typeofnode = createTypeof(array,integer,-1,low,NULL,-1);
                node->node.declareNode.typeOfId = *typeofnode;
                return typeofnode;
            }
            else if(strcmp(temp->node.typeNode.token,"REAL")==0)
            {
                Typeof* typeofnode = createTypeof(array,real,-1,low,NULL,-1);
                node->node.declareNode.typeOfId = *typeofnode;
                return typeofnode;
            }
            else if(strcmp(temp->node.typeNode.token,"BOOLEAN")==0)
            {
                Typeof* typeofnode = createTypeof(array,boolean,-1,low,NULL,-1);
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
            temp = temp->sc;
            int high = temp->sc->node.numNode.value;
            int low = temp->sc->rs->node.numNode.value;
            temp =temp->rs;
            if(strcmp(temp->node.typeNode.token,"INTEGER")==0)
            {
                Typeof* typeofnode = createTypeof(array,integer,high,low,NULL,-1);
                node->node.declareNode.typeOfId = *typeofnode;
                return typeofnode;
            }
            else if(strcmp(temp->node.typeNode.token,"REAL")==0)
            {
                Typeof* typeofnode = createTypeof(array,real,high,low,NULL,-1);
                node->node.declareNode.typeOfId = *typeofnode;
                return typeofnode;
            }
            else if(strcmp(temp->node.typeNode.token,"BOOLEAN")==0)
            {
                Typeof* typeofnode = createTypeof(array,boolean,high,low,NULL,-1);
                node->node.declareNode.typeOfId = *typeofnode;
                return typeofnode;
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
            Typeof* typeofnode = createTypeof(primitive,integer,-1,-1,NULL,-1);
            node->node.declareNode.typeOfId = *typeofnode;
            return typeofnode;
        }
        else if(strcmp(temp->node.typeNode.token,"REAL")==0)
        {
            Typeof* typeofnode = createTypeof(primitive,real,-1,-1,NULL,-1);
            node->node.declareNode.typeOfId = *typeofnode;
            return typeofnode;
        }
        else if(strcmp(temp->node.typeNode.token,"BOOLEAN")==0)
        {
            Typeof* typeofnode = createTypeof(primitive,boolean,-1,-1,NULL,-1);
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
                inputType = createTypeof(primitive, boolean, -1, -1, inputType, i);
            else if(strcmp(outputs->sc->rs->node.typeNode.token, "REAL")==0)
                inputType = createTypeof(primitive, real, -1, -1, inputType, i);
            else if(strcmp(outputs->sc->rs->node.typeNode.token, "INTEGER")==0)
                inputType = createTypeof(primitive, integer, -1, -1, inputType, i);
            else
                return NULL;
        }
        else if(inputs->sc->rs->type == arrayTypeNode){
            int lower = inputs->sc->rs->sc->sc->node.numNode.value;
            int higher = inputs->sc->rs->sc->sc->rs->node.numNode.value;
            if(strcmp(inputs->sc->rs->sc->rs->node.typeNode.token, "BOOLEAN")==0){
                if(inputs->sc->rs->sc->sc->type == numNode && inputs->sc->rs->sc->sc->rs->type == numNode)
                    inputType = createTypeof(array, boolean, lower, higher, inputType, i);
                else if(inputs->sc->rs->sc->sc->type == numNode && inputs->sc->rs->sc->sc->rs->type == idNode)
                    inputType = createTypeof(array, boolean, lower, -1, inputType, i);
                else if(inputs->sc->rs->sc->sc->type == idNode && inputs->sc->rs->sc->sc->rs->type == numNode)
                    inputType = createTypeof(array, boolean, -1, higher, inputType, i);
                else if(inputs->sc->rs->sc->sc->type == idNode && inputs->sc->rs->sc->sc->rs->type == idNode)
                    inputType = createTypeof(array, boolean, -1, -1, inputType, i);
                else
                    return NULL;
            }   
            else if(strcmp(inputs->sc->rs->sc->rs->node.typeNode.token, "REAL")==0){
                if(inputs->sc->rs->sc->sc->type == numNode && inputs->sc->rs->sc->sc->rs->type == numNode)
                    inputType = createTypeof(array, real, lower, higher, inputType, i);
                else if(inputs->sc->rs->sc->sc->type == numNode && inputs->sc->rs->sc->sc->rs->type == idNode)
                    inputType = createTypeof(array, real, lower, -1, inputType, i);
                else if(inputs->sc->rs->sc->sc->type == idNode && inputs->sc->rs->sc->sc->rs->type == numNode)
                    inputType = createTypeof(array, real, -1, higher, inputType, i);
                else if(inputs->sc->rs->sc->sc->type == idNode && inputs->sc->rs->sc->sc->rs->type == idNode)
                    inputType = createTypeof(array, real, -1, -1, inputType, i);
                else
                    return NULL;
            } 
            else if(strcmp(inputs->sc->rs->sc->rs->node.typeNode.token, "INTEGER")==0){
                if(inputs->sc->rs->sc->sc->type == numNode && inputs->sc->rs->sc->sc->rs->type == numNode)
                    inputType = createTypeof(array, integer, lower, higher, inputType, i);
                else if(inputs->sc->rs->sc->sc->type == numNode && inputs->sc->rs->sc->sc->rs->type == idNode)
                    inputType = createTypeof(array, integer, lower, -1, inputType, i);
                else if(inputs->sc->rs->sc->sc->type == idNode && inputs->sc->rs->sc->sc->rs->type == numNode)
                    inputType = createTypeof(array, integer, -1, higher, inputType, i);
                else if(inputs->sc->rs->sc->sc->type == idNode && inputs->sc->rs->sc->sc->rs->type == idNode)
                    inputType = createTypeof(array, integer, -1, -1, inputType, i);
                else
                    return NULL;
            }
            else
                return NULL;  
        }
        else
            return NULL;
        inputs = inputs->next;
    }

    //populate outputType array. NOTE: outputParameters cannot be Arrays
    for(i=0; i<no_of_outputs; i++){
        if(strcmp(outputs->sc->rs->node.typeNode.token, "BOOLEAN")==0)
            outputType = createTypeof(primitive, boolean, -1, -1, outputType, i);
        else if(strcmp(outputs->sc->rs->node.typeNode.token, "REAL")==0)
            outputType = createTypeof(primitive, real, -1, -1, outputType, i);
        else if(strcmp(outputs->sc->rs->node.typeNode.token, "INTEGER")==0)
            outputType = createTypeof(primitive, integer, -1, -1, outputType, i);
        else
            return NULL;
        outputs = outputs->next;
    }
    
    functionType->inputList = inputType;
    functionType->outputList = outputType;
    node->node.moduleNode.typeOfFunc = functionType;
    return functionType;
}

/*Checks whether arithmetic operation(PLUS,MINUS,DIV or MUL) can be performed on 2 operands of type t1 and t2*/
bool checkArithExpressionType(PrimitiveType t1, PrimitiveType t2){
    /*is integer, if both expressions are of type integer and the operator is arithmetic operator.
    is real, if both the expressions are of type real and the operator is arithmetic operator.
    is boolean, if both expressions are of type integer and the operator is a relational operator.
    is boolean, if both expressions are of type real and the operator is relational.
    is boolean, if both expressions are of type boolean and the operator is logical.*/

}

/*Checks whether relational(LE,LT,GT,GE,EQ or NE) can be performed on 2 operands of type t1,t2*/
bool checkRelationalExpressionType(PrimitiveType t1, PrimitiveType t2){

}

/*Checks whether logical(OR or AND) can be performed on 2 operands of type t1,t2*/
bool checkLogicalExpressionType(PrimitiveType t1, PrimitiveType t2){
    if(t1 == boolean && t2 == boolean)
        return true;
    else 
        return false;

    return false;
}


//extracts type of an expression, and also sets appropriate field in the AST's opNode
//param : node: ASTNode of type opNode or ASTNode of type idNode/numNode/boolNode
PrimitiveType extractTypeOfExpression(ASTNode *node)
{
    
    if(node->sc == NULL){   //leaf node case
        switch(node->type){
            case boolNode:{
                return boolean;
                break;
            }
            case numNode:{
                if(strcmp(node->node.numNode.token, "NUM")==0)
                    return integer;
                else
                    return real;                
                break;
            }
            case idNode:{
                //lookup type(obtain Typeof object)
                    //if lookup returns NULL(variable not delcared), semantic error
                    //else
                        //if Typeof.tag is primitive, no problem
                        //else if Typeof.tag is array, semantic error(mam's  rule 6.6)
                break;
            }
        }
    }

    ASTNode* left = node->sc;
    ASTNode* right = node->sc->rs;

    //arithmetic op case(mam's rule 6.1,6.2)
    if((strcmp(node->node.opNode.token,"PLUS")==0) || (strcmp(node->node.opNode.token,"MINUS")==0) || (strcmp(node->node.opNode.token,"MUL")==0) || (strcmp(node->node.opNode.token,"DIV")==0)){
        PrimitiveType t1 = extractTypeOfExpression(left);
        PrimitiveType t2 = extractTypeOfExpression(right);
        if((t1 == integer && t2 == integer) || (t1 == real && t2 == real))
            node->node.opNode.typeOfExpr = t1;
        else{
            //semantic error
        }
    }

    //logical op case(mam's rule 6.5)
    else if(strcmp(node->node.opNode.token, "OR")==0 || strcmp(node->node.opNode.token,"AND")==0){
        PrimitiveType t1 = extractTypeOfExpression(left);
        PrimitiveType t2 = extractTypeOfExpression(right);
        if(t1 == boolean && t2 == boolean)
            node->node.opNode.typeOfExpr = boolean;
        else{
            //semantic error
        }
    }

    //relational op case(mam's rule 6.3,6.4)
    else if(strcmp(node->node.opNode.token, "NE")==0 || strcmp(node->node.opNode.token, "EQ")==0 || strcmp(node->node.opNode.token, "LT")==0 || strcmp(node->node.opNode.token, "LE")==0 || strcmp(node->node.opNode.token, "GT")==0 || strcmp(node->node.opNode.token, "GE")==0){
        PrimitiveType t1 = extractTypeOfExpression(left);
        PrimitiveType t2 = extractTypeOfExpression(right);
        if((t1 == integer && t2 == integer) || (t1 == real && t2 == real))
            node->node.opNode.typeOfExpr = boolean;
        else{
            //semantic error
        }
    }
}

//extracts type of an expression, and also sets appropriate field in the AST's opNode
//param : node: ASTNode of type opNode
PrimitiveType extractTypeOfExpression(ASTNode *node)
{
    ASTNode* left = node->sc;
    ASTNode* right = node->sc->rs;

    //node is an arithmetic opNode
    if((strcmp(node->node.opNode.token,"PLUS")==0) || (strcmp(node->node.opNode.token,"MINUS")==0) || (strcmp(node->node.opNode.token,"MUL")==0) || (strcmp(node->node.opNode.token,"DIV")==0)){
        //if left child and right child are opnode
        if(left->type == opNode && right->type == opNode){
            //call extract type of expr on left and right
            PrimitiveType t1 = extractTypeOfExpression(left);
            PrimitiveType t2 = extractTypeOfExpression(right);
            //now check their types and assign appropriate type to node
            if(checkExpressionType(t1,t2,true)){
                
            }
            else{
                //semantic error
            }
        }
        
        //else if left child is opnode and right child is idnode/numnode
        else if(left->type == opNode && (right->type == idNode || right->type == numNode)){
            //callextract type of expr on left, hash right into table and obtain type
            PrimitiveType t1 = extractTypeOfExpression(left);
            PrimitiveType t2;
            if(right->type == idNode){   //need to extract type from symboltable
                //find from symboltable
            }
            else{       //right child is numNode, no need to extract
                if(strcmp(right->node.numNode.token, "NUM")==0)
                    t2 = integer;
                else
                    t2 = real;
            }
            //now check types and assign appropriately
        }
        
        //else if left child is idnode/numnode and right child is opnode
        else if((left->type == idNode || left->type == numNode) && left->type == opNode){
            //callextract type of expr on right, hash left into table and obtain type
            PrimitiveType t1;
            PrimitiveType t2 = extractTypeOfExpression(right);
            if(left->type == idNode)
            {
                // find from symboltable
            }
            else
            {
                if(strcmp(left->node.numNode.token,"NUM")==0)
                    t1 = integer;
                else
                    t1 = real;
                
            }
            //now check types and assign appropriately
        }
        
        //else if left child and right child are idnodes/numnodes
        else if((left->type == idNode && right->type == idNode) || (left->type == numNode && right->type == numNode)){
            //hash both into table and obtain type
            PrimitiveType t1;
            PrimitiveType t2;
            if(left->type == idNode)
            {
                // find from symboltable    
            }
            else
            {
                if(strcmp(left->node.numNode.token,"NUM")==0)
                    t1 = integer;
                else
                    t1 = real;
                
                if(strcmp(right->node.numNode.token,"NUM")==0)
                    t1 = integer;
                else
                    t1 = real;
            }   
            //set node's type accordingly
        }

        //else if left child is opnode and right child is nullnode 
        else if(left->type == opNode && right->type == nullNode){
            //call extract type of expr on left, assign this type to node
            PrimitiveType t1 = extractTypeOfExpression(left);
            node->node.opNode.typeOfExpr = t1;
            return t1;
        }
        
        //else if left child is idnode/numnode and right child is nullnode
        else if((left->type == idNode || left->type == numNode) && right->type == nullNode){
            //hash into symboltable and find idnodes type,
            PrimitiveType t1;
            PrimitiveType t2;
            if(left->type == idNode)
            {
                
            }
            else
            {
                if(strcmp(left->node.numNode.token,"NUM")==0)
                    t1 = integer;
                else
                    t1 = real;
            }
            
            // and assign this type to node
        }
    }

    //node is logical opNode
    else if(strcmp(node->node.opNode.token, "OR")==0 || strcmp(node->node.opNode.token,"AND")==0){  
        //here both the operands must be either boolean constants/expressions
        //if left and right childs are opnodes
            // call extract type on both left and right
            // check types and assign type to node accordingly
        //else if left child is opnode, right is boolnode
            //call extract type on left
            // check types and assign type to node accordingly
        //else if left child is boolnode, right child is opnode
            //call extract type on right
            // check types and assign type to node accordingly
        //else if left child and right are boolnodes
            //no need to check, just assign boolean to node
        //else
            //semantic error
    }

    //node is relationalopNode
    else if(strcmp(node->node.opNode.token, "NE")==0 || strcmp(node->node.opNode.token, "EQ")==0 || strcmp(node->node.opNode.token, "LT")==0 || strcmp(node->node.opNode.token, "LE")==0 || strcmp(node->node.opNode.token, "GT")==0 || strcmp(node->node.opNode.token, "GE")==0){
        //here both the operands must be integer/real constants/expressions
        //if
    }
}