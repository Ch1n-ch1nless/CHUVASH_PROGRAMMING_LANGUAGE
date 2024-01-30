#include "tree.h"

//=================================================================================================

TreeErrors TreeVerify(Tree* const tree)
{
    PTR_ASSERT(tree);

    TreeErrors error = TREE_ERR_NO;

    if (tree->root == nullptr)
    {
          return TREE_ERR_ROOT_IS_NULL;
    }

    return error;
}

//=================================================================================================

void PrintTreeErrors(Tree* const tree, TreeErrors error,    const char* const   file,
                                                            const int           line,
                                                            const char* const   function)
{
   PTR_ASSERT(tree);
   PTR_ASSERT(file);
   PTR_ASSERT(function);

   assert((line > 0) && "ERROR! Line is fewer than zero!!!\n");

   if (error & TREE_ERR_BAD_ALLOC)
   {
        printf("Error! Program can not allocate memory!!!\n");
   }

   if (error & TREE_ERR_ADD_ELEM)
   {
        printf("Error! Function can not add element to tree!!!\n");
   }

   if (error & TREE_ERR_ROOT_IS_NULL)
   {
        printf("Error! Root of tree is NULL!!!\n");
   }

   return;
}

//=================================================================================================

Node* NodeCtor(TreeErrors* const error, Token* const token)
{
    PTR_ASSERT(error);

    Node* new_node = nullptr;

    new_node = (Node*) calloc(1, sizeof(Node));
    if (new_node == nullptr)
    {
        *error = TREE_ERR_BAD_ALLOC;
        return new_node;
    }

    new_node->data  = token;

    new_node->left  = nullptr;

    new_node->right = nullptr;

    return new_node;
}

//=================================================================================================

TreeErrors TreeCtor(Tree* const tree)
{
    PTR_ASSERT(tree)

    TreeErrors error = TREE_ERR_NO;

    tree->root = NodeCtor(&error, nullptr);

    tree->size = 0;

    return error;
}

//=================================================================================================

TreeErrors SubTreeDtor(Node** const  node)
{
    PTR_ASSERT(node)
    PTR_ASSERT(*node)

    TreeErrors error = TREE_ERR_NO;

    if ((*node)->left != nullptr)
    {
        error = SubTreeDtor(&((*node)->left));
    }

    if ((*node)->right != nullptr)
    {
        error = SubTreeDtor(&((*node)->right));
    }

    free((*node)->data);

    free(*node);

    *node = nullptr;

    return error;
}

//=================================================================================================

TreeErrors TreeDtor(Tree* const tree)
{
    PTR_ASSERT(tree)

    TreeErrors error = TREE_ERR_NO;

    error = SubTreeDtor(&(tree->root));

    tree->size = 0;

    return error;
}

//=================================================================================================

TreeErrors TreeInsert(Tree* const tree, Token* const token, Node* const cur_node, AddNode indicator)
{
    PTR_ASSERT(tree)
    PTR_ASSERT(cur_node)

    TreeErrors error = TREE_ERR_NO;

    Node* new_node  = nullptr;

    new_node = NodeCtor(&error, token);

    if (cur_node == nullptr)
    {
        tree->root = new_node;
    }
    else
    {
        if (indicator == ADD_LEFT_NODE)
        {
            cur_node->left = new_node;
        }
        else if (indicator == ADD_RIGHT_NODE)
        {
            cur_node->right = new_node;
        }
        else
        {
            return TREE_ERR_UNDEFINIED_ID;
        }
    }

    return error;
}

//=================================================================================================

static Node* ReadNode(Buffer* const buffer, TknErrors* const error, NameSpace* const name_space)
{
    PTR_ASSERT(buffer)
    PTR_ASSERT(error)

    if (SkipWhiteSpaces(buffer) != STR_ERR_NO)
    {
        *error = TKN_ERR_WRONG_TYPE;
        return nullptr;
    }

    if (buffer->buffer[buffer->index] == '_')
    {
        buffer->index++;
        return nullptr;
    }

    if (buffer->buffer[buffer->index] != '(')
    {
        *error = TKN_ERR_WRONG_TYPE;
        return nullptr;
    }

    buffer->index++;

    if (SkipWhiteSpaces(buffer) != STR_ERR_NO)
    {
        *error = TKN_ERR_WRONG_TYPE;
        return nullptr;
    }

    Token* token = ReadToken(buffer, error, name_space);

    Node* node = NodeCtor(error, token);
    if (*error != TKN_ERR_NO)
        return nullptr;

    node->left  = ReadNode(buffer, error, name_space);
    node->right = ReadNode(buffer, error, name_space);

    if (buffer->buffer[buffer->index] != ')')
    {
        *error = TKN_ERR_WRONG_TYPE;
        return nullptr;
    }

    buffer->index++;

    return node;
}

//=================================================================================================

TknErrors ReadTree(Tree* const tree, NameSpace* const name_space)
{
    PTR_ASSERT(tree)

    Buffer* buffer = nullptr;
    StrErrors str_error = STR_ERR_NO;
    TknErrors tkn_error = TKN_ERR_NO;

    str_error = BufferCtor(buffer, SYNTAX_TREE_FILE);
    if (str_error != STR_ERR_NO)
    {
        PrintBufferError(str_error);
        return TKN_ERR_MEM_ALLOC;
    }

    tree->root = ReadNode(buffer, tkn_error, name_space);

    return tkn_error;
}

//=================================================================================================

static void PrintNode(FILE* const output, Node* const node)
{
    PTR_ASSERT(output)

    TknErrors tkn_error = TKN_ERR_NO;

    if (node == nullptr)
    {
        fprintf(output, "_ ");
        return;
    }

    fprintf(output, "( ");

    PrintToken(output, node->data, &tkn_error);
    if (tkn_error != TKN_ERR_NO)
    {
        fprintf(stderr, "ERROR! Program can not print this token!!!\n");
        return;
    }

    PrintNode(output, node->left);
    PrintNode(output, node->right);

    fprintf(") ");
}

//=================================================================================================

void PrintTree(Tree* const tree)
{
    PTR_ASSERT(tree)

    StrErrors   error   =   STR_ERR_NO;
    FILE*       output  =   nullptr;

    error = OpenFile(SYNTAX_TREE_FILE, &output, "w");
    if (error != STR_ERR_NO)
    {
        PrintBufferError(error);
        return;
    }

    PrintNode(output, tree->root);

    error = CloseFile(output);
    if (error != STR_ERR_NO)
    {
        PrintBufferError(error);
        return;
    }

    return;
}

//=================================================================================================