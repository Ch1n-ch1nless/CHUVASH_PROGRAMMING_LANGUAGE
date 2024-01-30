#ifndef LEXICAL_ANALYSIS_H_INCLUDED
#define LEXICAL_ANALYSIS_H_INCLUDED

#include "../../Tree/source/tree.h"
#include "../../Tree/source/tree_dump.h"
#include "../../List/source/list.h"

/*====================================STRUCTS_&_CONSTANTS====================================*/

enum FrontErrors
{
    FRONT_ERR_NO            = 0,
    FRONT_ERR_LEXICAL_ERROR = 1 << 0,
    FRONT_ERR_BAD_ALLOC     = 1 << 1,
    FRONT_ERR_EMPTY_DATA    = 1 << 2,
};

/*========================================FUNCTIONS==========================================*/

FrontErrors CreateTokenList(List* const token_list, Buffer* const buffer, NameSpace* const name_space);

/*===========================================================================================*/