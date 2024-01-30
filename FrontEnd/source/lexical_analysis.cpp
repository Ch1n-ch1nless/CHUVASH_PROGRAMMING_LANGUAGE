#include "lexical_analysis.h"

//=================================================================================================

static FrontErrors ReadStdKeyWords(Buffer* const buffer, NameSpace* const name_space, Token* const token)
{
    PTR_ASSERT(buffer)
    PTR_ASSERT(buffer->buffer)
    PTR_ASSERT(token)
    PTR_ASSERT(name_space)

    for (size_t i = 0; i < name_space->size; i++)
    {
        if (strncasecmp(buffer->buffer + buffer->index, name_space->array[i].name, strlen(name_space->array[i].name)) == 0)
        {
            token->type = name_space->array[i].type;
            token->value.bin_op = i;
            buffer->index += strlen(name_space->array[i].name);
            return FRONT_ERR_NO;
        }
    }

    token->type  = TYPE_UNDEFINED;
    token->value = TOKEN_POISON_VALUE;

    return FRONT_ERR_NO;
}

//=================================================================================================

static FrontErrors ReadNumber(Buffer* const buffer, NameSpace* const name_space, Token* const token)
{
    PTR_ASSERT(buffer)
    PTR_ASSERT(buffer->buffer)
    PTR_ASSERT(name_space)
    PTR_ASSERT(token)

    double number               = NAN;
    size_t number_of_symbols    = 0;

    if (token->type == TYPE_UNDEFINED)
    {
        if (sscanf(buffer->buffer + buffer->index, "%lg%n", &number, &number_of_symbols) == 1)
        {
            token->type         = TYPE_NUMBER;
            token->value.number = number;

            buffer->index += number_of_symbols
        }

        return FRONT_ERR_NO;
    }

    return FRONT_ERR_NO;
}

//=================================================================================================

static FrontErrors ReadName(Buffer* const buffer, NameSpace* const name_space, Token* const token)
{
    PTR_ASSERT(buffer)
    PTR_ASSERT(buffer->buffer)
    PTR_ASSERT(name_space)
    PTR_ASSERT(token)

    char    new_name[MAX_LEN_OF_WORD]   = {};
    size_t  index                       = 0;

    while (buffer->index < buffer->size && isalnum(buffer->buffer[buffer->index]))
    {
        new_name[index] = buffer->buffer[buffer->index];
        buffer->index++;
        index++;

        if (index >= MAX_LEN_OF_WORD && buffer->buffer[buffer->index] != '\0')
        {
            return FRONT_ERR_LEXICAL_ERROR;
        }
    }

    for (index = 0; index < name_space->size; index++)
    {
        if (strncasecmp(new_name, name_space->array[index].name, strlen(name_space->array[index].name)) == 0)
        {
            token->type = name_space->array[index].type;
            token->value.func_index = index;

            if (token->type != TYPE_VARIABLE && token->type != TYPE_FUNCTION)
            {
                return FRONT_ERR_LEXICAL_ERROR;
            }

            return FRONT_ERR_NO;
        }
    }

    if (name_space->size < name_space->capacity)
    {
        name_space->array[name_space->size].type = TYPE_VARIABLE;
        name_space->array[name_space->size].name = strdup(new_name);
        name_space->size++;
    }
    else
    {
        return FRONT_ERR_LEXICAL_ERROR;
    }

    return FRONT_ERR_NO;
}

//=================================================================================================

static FrontErrors ReadString(Buffer* const buffer, NameSpace* const name_space, Token* const token)
{
    PTR_ASSERT(buffer)
    PTR_ASSERT(buffer->buffer)
    PTR_ASSERT(name_space)
    PTR_ASSERT(token)

    token->type = TYPE_STRING;

    token->value.string = (char*) calloc (MAX_LEN_OF_WORD, sizeof(char));
    if (token->value.string == nullptr)
    {
        return FRONT_ERR_BAD_ALLOC;
    }

    size_t index = 0;

    while ((buffer->index < buffer->size) && (index < MAX_LEN_OF_WORD) && (buffer->buffer[buffer->index] != '\"'))
    {
        (token->value.string)[index] = buffer->buffer[buffer->index];
        index++;
        buffer->index++;
    }

    if (buffer->buffer[buffer->index] == '\"')
    {
        buffer->index++;
        return FRONT_ERR_NO;
    }
    
    return FRONT_ERR_LEXICAL_ERROR;
}

//=================================================================================================

static FindFunctionsInNameSpace(List* const token_list, NameSpace* const name_space)
{
    PTR_ASSERT(name_space)
    PTR_ASSERT(token_list)

    for (size_t i = 1; i < token_list->size; i++)
    {
        if (token_list->data[i-1].type == TYPE_KEY_OP && token_list->data[i-1].value.key_op == DEF)
        {
            if (token_list->data[i].type == TYPE_VARIABLE || token_list->data[i].type == TYPE_FUNCTION)
            {
                token_list->data[i].type = TYPE_FUNCTION;
                name_space->array[token_list->data[i].value.func_index].type = TYPE_FUNCTION;
            }
            else
            {
                return FRONT_ERR_LEXICAL_ERROR;
            }
        }

        if (token_list->data[i-1].type == TYPE_UN_OP && token_list->data[i-1].value.un_op == CALL)
        {
            if (token_list->data[i].type == TYPE_VARIABLE || token_list->data[i].type == TYPE_FUNCTION)
            {
                token_list->data[i].type = TYPE_FUNCTION;
                name_space->array[token_list->data[i].value.func_index].type = TYPE_FUNCTION;
            }
            else
            {
                return FRONT_ERR_LEXICAL_ERROR;
            }
        }
    }
}

//=================================================================================================

static FrontErrors SkipComments(Buffer* const buffer)
{
    PTR_ASSERT(buffer)
    PTR_ASSERT(buffer->buffer)

    if (buffer->buffer[buffer->index] == '#')
    {
        buffer->index++;

        while (buffer->index < buffer->size && buffer->buffer[buffer->index] != '#')
        {
            buffer->index++;
        }

        if (buffer->buffer[buffer->index] == '#')
        {
            buffer->index++;
            return FRONT_ERR_NO;
        }
        
        return FRONT_ERR_LEXICAL_ERROR;
    }

    return FRONT_ERR_NO;
}

//=================================================================================================

FrontErrors CreateTokenList(List* const token_list, Buffer* const buffer, NameSpace* const name_space)
{
    PTR_ASSERT(token_list)
    PTR_ASSERT(buffer)
    PTR_ASSERT(buffer->buffer)
    PTR_ASSERT(name_space)

    Token token = {};

    ListErrors  list_error  = LIST_ERR_NO;
    StrErrors   str_error   = STR_ERR_NO;

    while (buffer->index < buffer->size)
    {
        SkipWhiteSpaces(buffer);
        SkipComments(buffer);
        SkipWhiteSpaces(buffer);

        if (buffer->buffer[buffer->index] == '\"')
        {
            buffer->index++;
            ReadString(buffer, name_space, &token);
            continue;
        }

        ReadStdKeyWords(buffer, name_space, &token);
        ReadNumber(buffer, name_space, &token);
        ReadName(buffer, name_space, &token);

        if (token.type == TYPE_UNDEFINED)
        {
            return FRONT_ERR_LEXICAL_ERROR;
        }

        ListPushBack(token_list, token, &list_error)
    }

    FindFunctions(token_list, name_space);
}

//=================================================================================================