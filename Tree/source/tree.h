#ifndef TREE_H_INCLUDED
#define TREE_H_INCLUDED

#include "../../Tokens/source/tokens.h"

/*====================================STRUCTS_&_CONSTANTS====================================*/

struct Node
{
    Token*   data  = nullptr;
    Node*    left  = nullptr;
    Node*    right = nullptr;
};

struct Tree
{
    Node*    root  = nullptr;
    size_t   size  = 0;
};

enum AddNode : int
{
    ADD_LEFT_NODE  = -1,
    ADD_RIGHT_NODE =  1
};

enum TreeErrors : unsigned int
{
    TREE_ERR_NO             = 0,
    TREE_ERR_ROOT_IS_NULL   = 1 << 0,
    TREE_ERR_ADD_ELEM       = 1 << 1,
    TREE_ERR_BAD_ALLOC      = 1 << 2,
    TREE_ERR_UNDEFINIED_ID  = 1 << 3,
};

const char* const SYNTAX_TREE_FILE = "syntax_tree.fend"

/*========================================FUNCTIONS==========================================*/

Node*       NodeCtor(TreeErrors* const error, Token* const token);
TreeErrors  TreeCtor(Tree* const tree);

TreeErrors  TreeDtor(Tree* const tree);
TreeErrors  SubTreeDtor(Node** const node);

TreeErrors  TreeInsert(Tree* const tree, Token* const token, Node* const new_node, AddNode indicator);

TreeErrors  TreeVerify(Tree* const tree);

void        PrintTreeErrors(Tree* const tree, TreeErrors error,     const char* const file,
                                                                    const int         line,
                                                                    const char* const function);


TknErrors   ReadTree(Tree*  const tree);
void        PrintTree(Tree* const tree);

/*===========================================================================================*/

#endif // TREE_H_INCLUDED
