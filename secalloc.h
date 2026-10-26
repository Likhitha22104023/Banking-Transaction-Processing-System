#ifndef SECALLOC_H
#define SECALLOC_H
#include "Common.h"
typedef enum { BLOCK_ALLOCATED = 1, BLOCK_FREED } BlockState;
typedef struct BlockHeader {
    unsigned int magic;
    size_t size;
    BlockState state;
    char module[NAME_LENGTH];
    struct BlockHeader *next;
} BlockHeader;
void secureAllocInit(void);
void *secureAlloc(size_t size, const char *module);
ErrorCode secureFree(void *ptr);
bool validateBlock(const BlockHeader *block);
void detectMemoryLeaks(void);
void generateMemoryReport(void);
void secureAllocCleanup(void);
#endif
