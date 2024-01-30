#ifndef TOKENS_H_INCLUDED
#define TOKENS_H_INCLUDED

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "../../StringLib/source/string_lib.h"

/*====================================STRUCTS_&_CONSTANTS====================================*/

enum TknErrors : int
{
    TKN_ERR_NO          = 0,
    TKN_ERR_MEM_ALLOC   = 1 << 0,
    TKN_ERR_WRONG_TYPE  = 1 << 1,
};

enum Types : int
{
    TYPE_UNDEFINED  = -1,
    TYPE_PUNCT      =  0,
    TYPE_BIN_OP     =  1,
    TYPE_UN_OP      =  2,
    TYPE_KEY_OP     =  3,
    TYPE_NUMBER     =  4,
    TYPE_VARIABLE   =  5,
    TYPE_FUNCTION   =  6,
    TYPE_STRING     =  7,
};

/*~~~~~~~~~~~~~~~~~~~~~~~~~Codegeneration~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define DEF_UNARY(number, name, ...)        name = number, 

enum UnaryOperations : int
{
    #include "../../StdKeyWords/unary_operations.h"
}

#undef DEF_UNARY

/*-------------------------------------------------------*/

#define DEF_BINARY(number, name, ...)       name = number, 

enum BinaryOperations : int
{
    #include "../../StdKeyWords/binary_operations.h"
}

#undef DEF_BINARY

/*-------------------------------------------------------*/

#define DEF_KEY(number, name, ...)          name = number,

enum KeyWords : int
{
    #include "../../StdKeyWords/key_words.h"
}

#undef DEF_KEY

/*-------------------------------------------------------*/

#define DEF_PUNCT(name, number, ...)        name = number,

enum Punctuation : int
{
    #include "../../StdKeyWords/punctuation.h"
}

#undef DEF_PUNCT

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

union Value
{
    int     bin_op;
    int     un_op;
    int     key_op;
    double  number;
    int     var_index;
    int     punctuation;
    int     func_index;
    char*   string;
};

const Value TOKEN_POISON_VALUE = {.number = NAN};

struct Token
{
    Types   type    = TYPE_UNDEFINED;
    Value   value   = TOKEN_POISON_VALUE;
};

struct TokenInfo
{
    Types   type    = TYPE_UNDEFINED;
    char*   name    = nullptr;
};

struct NameSpace
{
    TokenInfo*  array       = nullptr;
    size_t      size        = 0;
    size_t      capacity    = 0;
};

const size_t MAX_SIZE_OF_CAPACITY = 1 << 10;

/*========================================FUNCTIONS==========================================*/

Token*      TokenCtor(TknErrors* const error);
TknErrors   TokenDtor(Token* const token);

NameSpace*  NameSpaceCtor(TknErrors* const error);
TknErrors   NameSpaceDtor(NameSpace* const name_space);

Token*      ReadToken(Buffer* const buffer, TknErrors* const error, NameSpace* name_space);
void        PrintToken(FILE* const output, Token* const token, TknErrors* const error);

/*===========================================================================================*/

#endif  // TOKENS_H_INCLUDED