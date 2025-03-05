#include <lib/MemLib.h>
#include <lib/Types.h>
#pragma once
#define HEAP_START 0x0700000
#define HEAP_SIZE (0x0D00000-0x0700000)

typedef struct HeapMemBlock {
    int size;
    void *ptr;
    struct HeapMemBlock *next;
    char free;
} HeapMemBlock;

extern unsigned char *Heap; // HEAP_START + sizeof(HeapMemBlock)
extern HeapMemBlock *HeapFree; // NULL
void HeapInit();
void *HeapAlloc(U32 size);
void HeapFreePtr(void *ptr);