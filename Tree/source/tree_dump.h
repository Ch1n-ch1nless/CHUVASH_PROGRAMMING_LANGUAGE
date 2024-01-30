#ifndef TREE_DUMP_H_INCLUDED
#define TREE_DUMP_H_INCLUDED

#include "tree.h"

/*====================================STRUCTS_&_CONSTANTS====================================*/

const char* const DOT_FILE = "tree.dot";

/*========================================FUNCTIONS==========================================*/

void TreeGraphDump(Tree* const tree, NameSpace* const name_space);

/*===========================================================================================*/

#endif // TREE_DUMP_H_INCLUDED