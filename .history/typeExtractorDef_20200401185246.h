/*
 * Group no. 50
 * Aniruddha Mahajan -------- 2017A7PS0145P
 * Ravindra Singh Shekhawat - 2017A7PS0146P
 * Shreyas Srikrishna ------- 2017A7PS0162P
 * Chetan Sharma ------------ 2017A7PS0182P
 */

#ifndef typeExtractorDef
#define typeExtractorDef

typedef enum Typeof{
    integer,
    real,
    boolean,
    dynamicArrayBool,
    dynamicArrayReal,
    dynamicArrayInt,
    staticArrayBool,
    staticArrayReal,
    staticArrayInt,
}Typeof;

typedef struct FunctionType{
    Typeof* inputList;      //array of types of input parameters
    Typeof* outputList;     //array of types of output parameters
}FunctionType;


#endif