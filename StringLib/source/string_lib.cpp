#include "string_lib.h"

//=================================================================================================

StrErrors OpenFile(const char* const file_name, FILE** const file, const char* const mode)
{
    PTR_ASSERT(file_name)
    PTR_ASSERT(file)
    PTR_ASSERT(mode)

    *file = fopen(file_name, mode);
    if (*file == nullptr)
        return STR_ERR_OPEN_FILE;

    return STR_ERR_NO;
}

//=================================================================================================

StrErrors CloseFile(const FILE* const file)
{
    PTR_ASSERT(file)

    if (fclose(file) != 0)
        return STR_ERR_CLOSE_FILE;

    return STR_ERR_NO;
}

//=================================================================================================

StrErrors SkipWhiteSpaces(char** const buffer)
{
    PTR_ASSERT( buffer)
    PTR_ASSERT(*buffer)

    size_t count_of_extra_char = 0;                 //Count of extra spaces, tabs before command
    char temp_array[MAX_LEN_OF_WORD] = {};          //Temporary array of symbols

    sscanf(*buffer, "%[ \t\n\r]%n", temp_array, &count_of_extra_char);
    
    *buffer = *buffer + count_of_extra_char;

    return STR_ERR_NO;
}

//=================================================================================================

StrErrors SkipWhiteSpaces(Buffer* const buffer)
{
    PTR_ASSERT(buffer)
    PTR_ASSERT(buffer->buffer)

    while (strchr(" \t\n\r", buffer->buffer[buffer->index]) != nullptr)
    {
        buffer->index++;
        if (buffer->index == buffer->size)
        {
            return STR_ERR_INDEX_IS_GREAT_SIZE;
        }
    }

    return STR_ERR_NO;
}

//=================================================================================================

void ClearBuffer(void)
{
    int c = EOF;
    do
    {
        c = getchar();
    } 
    while (c != EOF && c != '\n');
}

//=================================================================================================

static StrErrors CountBufferSize(const char* const file_name, Buffer* const buffer)
{
    PTR_ASSERT(file_name)
    PTR_ASSERT(buffer)

    struct stat st = {};

    stat(file_name, &st);

    if (st.size == 0)
    {
        return STR_ERR_FILE_IS_EMPTY;
    }

    buffer->size = st.st_size;

    return STR_ERR_NO;
}

//=================================================================================================

static StrErrors ReadBufferFromFile(FILE* const file_ptr, Buffer* const buffer)
{
    PTR_ASSERT(file_ptr)
    PTR_ASSERT(buffer)

    size_t number_of_symbols = fread(buffer->buffer, sizeof(char), buffer->size, file_ptr);

    if (number_of_symbols != buffer->size) 
    {
        if (feof(file_ptr)) 
        {
            printf("Error reading %s: unexpected end of file\n", "<STRING>");
            return STR_ERR_FREAD;
        } 
        else if (ferror(file_ptr)) 
        {
            printf("Error reading %s", "<STRING>");
            return STR_ERR_FREAD;
        }
        
        if (number_of_symbols > buffer->size) 
        {
            printf("Error! Symbols in file  more then buffer size!");
            assert(false);
        }
    }

    return STR_ERR_NO;
}

//=================================================================================================

StrErrors BufferCtor(Buffer* const buffer, const char* const file_name)
{
    PTR_ASSERT(buffer)

    StrErrors error = STR_ERR_NO;

    error = CountBufferSize(file_name, buffer);
    if (error != STR_ERR_NO)
        return error;

    buffer->buffer = (char*) calloc(buffer->size + 1, sizeof(char));
    if (buffer->buffer == nullptr)
        return STR_ERR_MEM_ALLOC;

    FILE* file_ptr = nullptr;
    error = OpenFile(file_name, &file_ptr, "rb");
    if (error != STR_ERR_NO)
        return error;

    error = ReadBufferFromFile(file_ptr, buffer);

    return error;
}

//=================================================================================================

StrErrors BufferDtor(Buffer* const buffer)
{
    PTR_ASSERT(buffer)

    free(buffer->buffer);
    buffer->buffer = nullptr;
    buffer->size   = 0;

    return STR_ERR_NO;
}

//=================================================================================================

StrErrors BufferVerify(Buffer* const buffer)
{
    PTR_ASSERT(buffer)

    StrErrors error = STR_ERR_NO;

    if (buffer->buffer == nullptr)
    {
        error = STR_ERR_PTR_IS_NULL;
        return error;
    }

    if (buffer->size < 0)
    {
        error = STR_ERR_SIZE_IS_FEWER_ZERO;
        return error;
    }

    return error;
}

//=================================================================================================

void PrintBufferError(StrErrors error)
{
    switch(error)
    {
        case STR_ERR_NO:
            break;

        case STR_ERR_CLOSE_FILE:
        {
            printf("ERROR!!! Program can not close the file!\n");
            break;
        }

        case STR_ERR_FILE_IS_EMPTY:
        {
            printf("ERROR!!! File is empty! Please, fill it!\n");
            break;
        }

        case STR_ERR_FREAD:
        {
            printf("ERROR!!! In function \"fread()\" ¯\\_(ツ)_/¯\n");
            break;
        }

        case STR_ERR_MEM_ALLOC:
        {
            printf("ERROR!!! Program can not open allocate memory!\n");
            break;
        }

        case STR_ERR_OPEN_FILE:
        {
            printf("ERROR!!! Program can not open file!\n");
            break;
        }

        case STR_ERR_PTR_IS_NULL:
        {
            printf("ERROR!!! In struct \'Buffer\' poiter to array is NULL!\n");
            break;
        }

        case STR_ERR_SIZE_IS_FEWER_ZERO:
        {
            printf("ERROR!!! Size of array is fewer then zero!\n");
        }

        default:
            break;
    }
}

//=================================================================================================