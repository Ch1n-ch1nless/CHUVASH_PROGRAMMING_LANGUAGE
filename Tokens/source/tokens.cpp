#include "tokens.h"

//=================================================================================================

Token* TokenCtor(TknErrors* error)
{
    PTR_ASSERT(error)

    Token* token = (Token*) calloc(1, sizeof(Token));
    if (token == nullptr)
    {
        *error = TKN_ERR_MEM_ALLOC
        return nullptr;
    }

    token->type  = TYPE_UNDEFINED;
    token->value = TOKEN_POISON_VALUE;

    return token;
} 

//=================================================================================================

TknErrors TokenDtor(Token* token)
{
    PTR_ASSERT(token)

    TknErrors error = TKN_ERR_NO;

    free(token);

    return error;
}

//=================================================================================================

NameSpace* NameSpaceCtor(TknErrors* const error)
{
    PTR_ASSERT(error)

    NameSpace* name_space = (NameSpace*) calloc(1, sizeof(NameSpace));
    if (name_space == nullptr)
    {
        *error = TKN_ERR_MEM_ALLOC;
        return nullptr;
    }

    name_space->capacity = MAX_SIZE_OF_CAPACITY;
    name_space->size     = 0;

    name_space->name_array = (TokenInfo*) calloc(MAX_SIZE_OF_CAPACITY, sizeof(TokenInfo));
    if (name_space->name_array == nullptr)
    {
        *error = TKN_ERR_MEM_ALLOC;
        free(name_space);
        return nullptr;
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    #define DEF_BINARY(number, name, lang_name, ...)        \
        name_space->array[number].name = strdup(lang_name); \
        name_space->array[number].type = TYPE_BIN_OP;       \
        name_space->size++;                                 \
        if (name_space->name_array[number] == nullptr)      \
        {                                                   \
            *error = TKN_ERR_MEM_ALLOC;                     \
            free(name_space);                               \
            return nullptr;                                 \
        }

    #include "../../StdKeyWords/binary_operations.h"            /*Insert Binary operations to new name space!*/

    #undef DEF_BINARY      

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    #define DEF_UNARY(number, name, lang_name, ...)         \
        name_space->array[number].name = strdup(lang_name); \
        name_space->array[number].type = TYPE_UN_OP;        \
        name_space->size++;                                 \
        if (name_space->name_array[number] == nullptr)      \
        {                                                   \
            *error = TKN_ERR_MEM_ALLOC;                     \
            free(name_space);                               \
            return nullptr;                                 \
        }

    #include "../../StdKeyWords/unary_operations.h"            /*Insert Unary operations to new name space!*/

    #undef DEF_UNARY      

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    #define DEF_KEY(number, name, lang_name, ...)           \
        name_space->array[number].name = strdup(lang_name); \
        name_space->array[number].type = TYPE_KEY_OP;       \
        name_space->size++;                                 \
        if (name_space->name_array[number] == nullptr)      \
        {                                                   \
            *error = TKN_ERR_MEM_ALLOC;                     \
            free(name_space);                               \
            return nullptr;                                 \
        }

    #include "../../StdKeyWords/key_words.h"                    /*Insert Key words to new name space!*/

    #undef DEF_KEY     

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    #define DEF_PUNCT(number, name, lang_name, ...)         \
        name_space->array[number].name = strdup(lang_name); \
        name_space->array[number].type = TYPE_PUNCT;        \
        name_space->size++;                                 \
        if (name_space->name_array[number] == nullptr)      \
        {                                                   \
            *error = TKN_ERR_MEM_ALLOC;                     \
            free(name_space);                               \
            return nullptr;                                 \
        }

    #include "../../StdKeyWords/punctuation.h"            /*Insert Punctuation to new name space!*/

    #undef DEF_PUNCT     

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    return name_space;
}

//=================================================================================================

TknErrors NameSpaceDtor(NameSpace* name_space)
{
    PTR_ASSERT(name_space)

    for (size_t index = 0; index < name_space->size; index++)
    {
        free(name_space->array[index].name);
        free(name_space->array[index]);
    }

    free(name_space->array);
    free(name_space);

    return TKN_ERR_NO;
}

//=================================================================================================

static void AddToNameSpace(NameSpace* const name_space, const Types type, const int index)
{
    PTR_ASSERT(name_space)
    assert((index >= name_space->capacity) && "ERROR!!! Index is more than capacity!\n");

    name_space->array[index].type = type;

    char tmp_name[MAX_LEN_OF_WORD] = {};

    if (type == TYPE_FUNCTION)
    {
        sprintf(tmp_name, "_func_%d", index);
    }
    else
    {
        sprintf(tmp_name, "_X_%d", index);
    }

    name_space->array[index].name = strdup(tmp_name);
}

//=================================================================================================

static int ReadIndexInNameSpace(Buffer* const buffer, TknErrors* const error, NameSpace* name_space)
{
    PTR_ASSERT(buffer)
    PTR_ASSERT(buffer->buffer)
    PTR_ASSERT(error)
    PTR_ASSERT(name_space)

    int index                   = -1;
    size_t number_of_symbols    =  0;

    if (sscanf(buffer->buffer + buffer->index, "%d%n", &index, &number_of_symbols) != 1)
    {
        *error = TKN_ERR_WRONG_TYPE;
        return -1;
    }

    if (index < 0 || index > name_space->capacity)
    {
        *error = TKN_ERR_WRONG_TYPE;
        return -1;
    }

    buffer->index += number_of_symbols;

    return index;
}

//=================================================================================================

static double ReadDoubleNumber(Buffer* const buffer, TknErrors* const error, NameSpace* name_space)
{
    PTR_ASSERT(buffer)
    PTR_ASSERT(buffer->buffer)
    PTR_ASSERT(error)
    PTR_ASSERT(name_space)

    double number               = NAN;
    size_t number_of_symbols    = 0;

    if (sscanf(buffer->buffer + buffer->index, "%lg%n", &number, &number_of_symbols) != 1)
    {
        *error = TKN_ERR_WRONG_TYPE;
        return -1;
    }

    if (number == NAN)
    {
        *error = TKN_ERR_WRONG_TYPE;
        return -1;
    }

    buffer->index += number_of_symbols;
    
    return number;
}

//=================================================================================================

static char* ReadString(Buffer* const buffer, TknErrors* const error, NameSpace* name_space)
{
    PTR_ASSERT(buffer)
    PTR_ASSERT(buffer->buffer)
    PTR_ASSERT(error)
    PTR_ASSERT(name_space)

    char* string = (char*) calloc(MAX_LEN_OF_WORD, sizeof(char));
    if (string == nullptr)
    {
        *error = TKN_ERR_MEM_ALLOC;
        return nullptr;
    }

    size_t number_of_symbols = 0;

    while (buffer->buffer[buffer->index] != '\"' && buffer->index < buffer->size)
    {
        string[number_of_symbols] = buffer->buffer[buffer->index];
        buffer->index++;
    }

    if (buffer->index >= buffer->size)
    {
        *error = TKN_ERR_WRONG_TYPE;
        printf("ERROR!!! Index is greater than size of buffer!\n")
        return nullptr;
    }

    if (buffer->buffer[buffer->index] != '\"')
    {
        *error = TKN_ERR_WRONG_TYPE;
        printf("ERROR!!! String does not have the end!\n")
        return nullptr;
    }

    buffer->index++;
    return string;
}

//=================================================================================================

Token* ReadToken(Buffer* const buffer, TknErrors* const error, NameSpace* name_space)
{
    PTR_ASSERT(buffer)
    PTR_ASSERT(buffer->buffer)
    PTR_ASSERT(error)

    Token* token = TokenCtor(error);
    if (*error != TKN_ERR_NO)
        return nullptr;

    int     type                = TYPE_UNDEFINED;
    size_t  number_of_symbols   = 0;

    if (SkipWhiteSpaces(buffer) != STR_ERR_NO)
    {
        fprintf(stderr, "ERROR!!! In function \'SkipWhiteSpaces()\'\n");
        return nullptr;
    } 

    if (sscanf(buffer->buffer + buffer->index, "%d%n", &type, &number_of_symbols) != 1)
    {
        *error = TKN_ERR_WRONG_TYPE;
        return nullptr;
    }

    token->type = type;

    switch (type)
    {
        case TYPE_PUNCT:
        case TYPE_BIN_OP:
        case TYPE_FUNCTION:
        case TYPE_KEY_OP:
        case TYPE_UN_OP:
        case TYPE_VARIABLE:
        {
            token->value.bin_op = ReadIndexInNameSpace(buffer, error, name_space);
            if (*error != TKN_ERR_NO)
                return nullptr;
            break;
        }

        case TYPE_NUMBER:
        {
            token->value.number = ReadDoubleNumber(buffer, error, name_space.);
            if (*error != TKN_ERR_NO)
                return nullptr;
            break;
        }
        
        case TYPE_STRING:
        {
            token->value.string = ReadString(buffer, error, name_space);
            if (*error != TKN_ERR_NO)
                return nullptr;
            break;
        }

        case TYPE_UNDEFINED:
        default:
        {
            *error = TKN_ERR_WRONG_TYPE;
            return nullptr;
        }
    }

    if (token->type == TYPE_FUNCTION || token->type == TYPE_VARIABLE)
    {
        AddToNameSpace(name_space, token->type, token->value.var_index);
    }

    return token;
}

//=================================================================================================

void PrintToken(FILE* const output, Token* const token, TknErrors* const error)
{
    PTR_ASSERT(output)
    PTR_ASSERT(token)
    PTR_ASSERT(error)

    fprintf(output, "%d ", token->type);

    switch (token->type)
    {
        case TYPE_PUNCT:
        {
            fprintf(output, "%d ", token->value.punctuation);
            break;
        }

        case TYPE_BIN_OP:
        {
            fprintf(output, "%d ", token->value.bin_op);
            break;
        }

        case TYPE_UN_OP:
        {
            fprintf(output, "%d ", token->value.un_op);
            break;
        }

        case TYPE_KEY_OP:
        {
            fprintf(output, "%d ", token->value.key_op);
            break;
        }

        case TYPE_NUMBER:
        {
            fprintf(output, "%lg ", token->value.number);
            break;
        }

        case TYPE_VARIABLE:
        {
            fprintf(output, "%d ", token->value.var_index);
            break;
        }

        case TYPE_FUNCTION:
        {
            fprintf(output, "%d ", token->value.func_index);
            break;
        }

        case TYPE_STRING:
        {
            fprintf(output, "\"%s\" ", token->value.string);
            break;
        }
        
        case TYPE_UNDEFINED:
        default:
        {
            *error = TKN_ERR_WRONG_TYPE;
            return;
        }
    }

    return;
}

//=================================================================================================