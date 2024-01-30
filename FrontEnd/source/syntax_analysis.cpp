#include "syntax_analysis.h"

static Node* GetGeneral(List* const token_list, NameSpace* const name_space, size_t* const index, FrontErrors* const error);
static Node* GetFunction(List* const token_list, NameSpace* const name_space, size_t* const index, FrontErrors* const error);
static Node* GetBody(List* const token_list, NameSpace* const name_space, size_t* const index, FrontErrors* const error);
static Node* GetIf(List* const token_list, NameSpace* const name_space, size_t* const index, FrontErrors* const error);
static Node* GetAssignment(List* const token_list, NameSpace* const name_space, size_t* const index, FrontErrors* const error);
static Node* GetWhile(List* const token_list, NameSpace* const name_space, size_t* const index, FrontErrors* const error);
static Node* GetComparison(List* const token_list, NameSpace* const name_space, size_t* const index, FrontErrors* const error);
static Node* GetCondition(List* const token_list, NameSpace* const name_space, size_t* const index, FrontErrors* const error);
static Node* GetExpression(List* const token_list, NameSpace* const name_space, size_t* const index, FrontErrors* const error);
static Node* GetTerm(List* const token_list, NameSpace* const name_space, size_t* const index, FrontErrors* const error);
static Node* GetPow(List* const token_list, NameSpace* const name_space, size_t* const index, FrontErrors* const error);
static Node* GetUnaryOperator(List* const token_list, NameSpace* const name_space, size_t* const index, FrontErrors* const error);
static Node* GetPrimaryExpression(List* const token_list, NameSpace* const name_space, size_t* const index, FrontErrors* const error);
static Node* GetNumber(List* const token_list, NameSpace* const name_space, size_t* const index, FrontErrors* const error);
static Node* GetVariable(List* const token_list, NameSpace* const name_space, size_t* const index, FrontErrors* const error);

//=================================================================================================

FrontErrors CreateTreeFromTokenList(Tree* const tree, List* const token_list, NameSpace* const name_space)
{
    PTR_ASSERT(tree)
    PTR_ASSERT(token_list)
    PTR_ASSERT(name_space)

    FrontErrors error = FRONT_ERR_NO;
    size_t      index = 1;

    tree->root = GetGeneral(token_list, name_space, &index, &error);

    return error;
}

//=================================================================================================

static bool IsFunction(List * const token_list, size_t* const index)
{
    PTR_ASSERT(token_list)
    PTR_ASSERT(index)

    if (token_list->data[*index].type == TYPE_KEY_OP && token_list->data[*index].value.key_op == DEF)
    {
        return true;
    }

    return false;
}

//=================================================================================================

static Node* GetGeneral(List* const token_list, NameSpace* const name_space, size_t* const index, FrontErrors* const error)
{
    PTR_ASSERT(token_list)
    PTR_ASSERT(name_space)
    PTR_ASSERT(index)
    PTR_ASSERT(error)

    Node* main_func = GetFunction(token_list, name_space, index, error);
    Node* next_func = main_func;

    while (IsFunction(token_list, index))
    {
        next_func->right = GetFunction(token_list, name_space, index, error);
        next_func = next_func->right;
    }

    return main_func;
}

//=================================================================================================

static Node* GetFunction(List* const token_list, NameSpace* const name_space, size_t* const index, FrontErrors* const error)
{
    PTR_ASSERT(token_list)
    PTR_ASSERT(name_space)
    PTR_ASSERT(index)
    PTR_ASSERT(error)

    if (IsFunction(token_list, index) == false)
    {
        *error = FRONT_ERR_LEXICAL_ERROR;
        return nullptr;
    }

    (*index)++;

    Node* func_name = GetVariable(token_list, name_space, index, error);

    if (IsBracketOpen(token_list, index))
    {
        (*index)++;
    }
    Node* func_args = GerArgs(token_list, name_space, index, error);
    Node* func_body = GetBody(token_list, name_space, index, error);

    Token token = {.type = TYPE_PUNCT, .value.punctuation = END_OF_OPERATION};
    TknErrors tkn_error = TKN_ERR_NO;

    Node* punct_node = NodeCtor(&tkn_error, &token);

    punct_node->right   = func_args;
    punct_node->left    = func_body;

    func_name->left     = punct_node;

    return func_name;
}

//=================================================================================================

static Node* GetArgs(List* const token_list, NameSpace* const name_space, size_t* const index, FrontErrors* const error)
{
    PTR_ASSERT(token_list)
    PTR_ASSERT(name_space)
    PTR_ASSERT(index)
    PTR_ASSERT(error)

    TknErrors tkn_error = TKN_ERR_NO;

    Node*   begin_node  = nullptr;
    Node*   new_node    = nullptr;
    Node*   cur_node    = nullptr;

    if (token_list->data[*index].type == TYPE_PUNCT && token_list->data[*index].value.punctuation == OPEN_PARANTHESES)
    {
        (*index)++;

        Token* new_token = TokenCtor(&tkn_error);
        new_token->type  = TYPE_PUNCT;
        new_token->value = END_OF_OPERATION;

        begin_node = NodeCtor(&tkn_error, new_token);

        while ( token_list->data[*index].type == TYPE_PUNCT && token_list->data[*index].value.punctuation != CLOSE_PARANTHESES )
        {
            new_node = GetExpression(token_list, name_space, index, error);

            if (cur_node == nullptr)
            {
                begin_node->left    =   new_node;
                cur_node            =   begin_node
            }
            else
            {
                cur_node->right = NodeCtor(&tkn_error, new_token);
                cur_node = cur_node->right;
            }

            if (token_list->data[*index].type == TYPE_PUNCT && token_list->data[*index].value.punctuation == COMMA)
            {
                (*index)++;
            }
            else
            {
                break;
            }
        }

        if (token_list->data[*index].type == TYPE_PUNCT && token_list->data[*index].value == CLOSE_PARANTHESES)
        {
            (*index)++;
        }
        else
        {
            *error = FRONT_ERR_LEXICAL_ERROR;
            return nullptr;
        }
         
    }

    return begin_node;
}

//=================================================================================================

static Node* GetBody(List* const token_list, NameSpace* const name_space, size_t* const index, FrontErrors* const error)
{
    PTR_ASSERT(token_list)
    PTR_ASSERT(name_space)
    PTR_ASSERT(index)
    PTR_ASSERT(error)

    TknErrors tkn_error = TKN_ERR_NO;

    if (token_list->data[*index].type == TYPE_PUNCT && token_list->data[*index].value.punctuation == OPEN_BRACE)
    {
        (*index)++;

        Node* ret_node = nullptr;
        Node* cur_node = nullptr;
        Node* new_node = nullptr;

        Token* punct_token = TokenCtor(&tkn_error);

        punct_token->type               = TYPE_PUNCT;
        punct_token->value.punctuation  = END_OF_OPERATION;

        while (!(token_list->data[*index].type == TYPE_PUNCT && token_list->data[*index].value.punctuation == CLOSE_BRACE))
        {
            new_node = GetOperation(token_list, name_space, index, error);

            if (ret_node == nullptr)
            {
                ret_node = NodeCtor(&tkn_error, punct_token);
                ret_node->left = new_node;
                cur_node = ret_node;
            }
            else
            {
                cur_node->right = NodeCtor(&tkn_error, punct_token);
                cur_node->left  = new_node;
                cur_node = cur_node->right;
            }
        }
        
        (*index)++;

        return ret_node;

    }

    *error = FRONT_ERR_LEXICAL_ERROR;
    return nullptr;
}

//=================================================================================================

static Node* GetOperation(List* const token_list, NameSpace* const name_space, size_t* const index, FrontErrors* const error)
{
    PTR_ASSERT(token_list)
    PTR_ASSERT(name_space)
    PTR_ASSERT(index)
    PTR_ASSERT(error)

    TknErrors   tkn_error   = TKN_ERR_NO;
    Token*      token       = TokenCtor(&tkn_error);

    token->type     = TYPE_UNDEFINED;
    token->value    = TOKEN_POISON_VALUE;

    Node* new_node = nullptr;
    Node* ret_node = nullptr;

    if (token_list->data[*index].type == TYPE_UN_OP)
    {
        (*index)++;
        token->type = TYPE_UN_OP;
        token->value.un_op = *index;

        new_node = GetExpression(token_list, name_space, index, error);

        ret_node = NodeCtor(&tkn_error, token);
        ret_node->right = new_node;
    }
    else if (token_list->data[*index].type == TYPE_KEY_OP)
    {
        (*index)++;
        token->type         = TYPE_KEY_OP;
        token->value.key_op = *index;

        switch(token->value.key_op)
        {
            case IF:
                new_node = GetIf(token_list, name_space, index, error);
                break;

            case WHILE:
                new_node = GetWhile(token_list, name_space, index, error);
                break;
            
            default:
                break;
        }
    }
    else if (token_list->data[*index].type == TYPE_VARIABLE)
    {
        cur_node = GetAssume(token_list, name_space, index, error);
    }
    else if (token_list->data[*index].type == TYPE_FUNCTION)
    {
        token->type             = TYPE_FUNCTION;
        token->value.func_index = *index;


    }
    else
    {
        *error = FRONT_ERR_LEXICAL_ERROR;
        
    }
}
