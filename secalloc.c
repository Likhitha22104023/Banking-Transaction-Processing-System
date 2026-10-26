#include "secalloc.h"
#include <stdint.h>
#include <inttypes.h>
#define SECALLOC_MAGIC 0x53414C4CU
#define SECALLOC_CANARY UINT64_C(0xA5C39E7B12D468F0)
static BlockHeader *blockList = NULL;
static size_t currentBytes = 0, peakBytes = 0;
static size_t totalAllocations = 0, totalFrees = 0, failedAllocations = 0;
static void writeCanary(unsigned char *address)
{
    memcpy(address, &(uint64_t){SECALLOC_CANARY}, sizeof(uint64_t));
}
static bool checkCanary(const unsigned char *address)
{
    uint64_t value;
    memcpy(&value, address, sizeof(value));
    return value == SECALLOC_CANARY;
}
static BlockHeader *findBlock(void *ptr)
{
    for (BlockHeader *block = blockList; block != NULL; block = block->next)
        if ((void *)(block + 1) == ptr) return block;
    return NULL;
}
void secureAllocInit(void)
{
    blockList = NULL; currentBytes = 0; peakBytes = 0;
    totalAllocations = 0; totalFrees = 0; failedAllocations = 0;
}
bool validateBlock(const BlockHeader *block)
{
    const unsigned char *userData;
    if (block == NULL || block->magic != SECALLOC_MAGIC ||
        block->state != BLOCK_ALLOCATED) return false;
    userData = (const unsigned char *)(block + 1);
    return checkCanary(userData + block->size);
}
void *secureAlloc(size_t size, const char *module)
{
    BlockHeader *block;
    size_t totalSize;
    unsigned char *userData;
    if (size == 0 || size > SIZE_MAX - sizeof(BlockHeader) - sizeof(uint64_t)) {
        failedAllocations++; return NULL;
    }
    totalSize = sizeof(BlockHeader) + size + sizeof(uint64_t);
    block = (BlockHeader *)malloc(totalSize);
    if (block == NULL) { failedAllocations++; return NULL; }
    block->magic = SECALLOC_MAGIC; block->size = size;
    block->state = BLOCK_ALLOCATED;
    snprintf(block->module, sizeof(block->module), "%s",
             module != NULL ? module : "Unknown");
    block->next = blockList; blockList = block;
    userData = (unsigned char *)(block + 1);
    writeCanary(userData + size);
    currentBytes += size;
    if (currentBytes > peakBytes) peakBytes = currentBytes;
    totalAllocations++;
    return userData;
}
ErrorCode secureFree(void *ptr)
{
    BlockHeader *block;
    if (ptr == NULL) return E_SUCCESS;
    block = findBlock(ptr);
    if (block == NULL) return E006_INVALID_FREE;
    if (block->state == BLOCK_FREED) return E007_DOUBLE_FREE;
    if (!validateBlock(block)) return E008_MEMORY_CORRUPTION;
    block->state = BLOCK_FREED; block->magic = 0;
    if (currentBytes >= block->size) currentBytes -= block->size;
    else currentBytes = 0;
    totalFrees++;
    return E_SUCCESS;
}
void detectMemoryLeaks(void)
{
    size_t leaks = 0;
    printf("\\n--- Memory Leak Report ---\\n");
    for (BlockHeader *block = blockList; block != NULL; block = block->next) {
        if (block->state == BLOCK_ALLOCATED) {
            printf("Leak: %zu bytes, module: %s\\n", block->size, block->module);
            leaks++;
        }
    }
    if (leaks == 0) printf("No tracked memory leaks detected.\\n");
    else printf("Outstanding blocks: %zu\\n", leaks);
}
void generateMemoryReport(void)
{
    printf("\\n--- SecureAlloc Memory Report ---\\n");
    printf("Current allocated bytes: %zu\\n", currentBytes);
    printf("Peak allocated bytes: %zu\\n", peakBytes);
    printf("Total allocations: %zu\\n", totalAllocations);
    printf("Total frees: %zu\\n", totalFrees);
    printf("Failed allocations: %zu\\n", failedAllocations);
}
void secureAllocCleanup(void)
{
    BlockHeader *block = blockList;
    while (block != NULL) {
        BlockHeader *next = block->next;
        free(block); block = next;
    }
    blockList = NULL; currentBytes = 0;
}
