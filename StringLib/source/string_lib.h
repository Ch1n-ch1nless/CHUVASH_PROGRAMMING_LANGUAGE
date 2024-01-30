#ifndef STRING_LIB_H_INCLUDED
#define STRING_LIB_H_INCLUDED

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/*==========================================DEFINE===========================================*/

#define PTR_ASSERT(ptr) assert((ptr != nullptr) && "ERROR!!! Pointer to " #ptr " is NULL!!!\n");

/*====================================STRUCTS_&_CONSTANTS====================================*/

enum StrErrors
{
    STR_ERR_NO                  = 0,
    STR_ERR_OPEN_FILE           = 1 << 0,
    STR_ERR_CLOSE_FILE          = 1 << 1,
    STR_ERR_MEM_ALLOC           = 1 << 2,
    STR_ERR_PTR_IS_NULL         = 1 << 3,
    STR_ERR_FILE_IS_EMPTY       = 1 << 4,
    STR_ERR_FREAD               = 1 << 5,
    STR_ERR_SIZE_IS_FEWER_ZERO  = 1 << 6,
    STR_ERR_INDEX_IS_GREAT_SIZE = 1 << 7,
};

const size_t MAX_LEN_OF_WORD = 1 << 10;

struct Buffer
{
    char*   buffer  = nullptr;
    size_t  index   = 0;
    size_t  size    = 0;
};

/*========================================FUNCTIONS==========================================*/

StrErrors   OpenFile(const char* const file_name, FILE** const file, const char* const mode);
StrErrors   CloseFile(const FILE* const file);

StrErrors   SkipWhiteSpaces(char** const buffer);
StrErrors   SkipWhiteSpaces(Buffer* const buffer);
void        ClearBuffer(void);

StrErrors   BufferCtor(Buffer* const buffer, const char* const file_name);
StrErrors   BufferDtor(Buffer* const buffer);

StrErrors   BufferVerify(Buffer* const buffer);
void        PrintBufferError(StrErrors error);

/*===========================================================================================*/

#endif // STRING_LIB_H_INCLUDED