#include <kernel/mem.h>
#pragma once
#ifndef NULL
#define NULL 0
#endif
#define HEAP_START 0x3000F0
#define HEAP_SIZE 0x10000

typedef struct HeapMemBlock {
    int size;
    void *ptr;
    struct HeapMemBlock *next;
    char free;
} HeapMemBlock;

extern unsigned char *Heap; // HEAP_START + sizeof(HeapMemBlock)
extern HeapMemBlock *HeapFree; // NULL
void HeapInit();
void *HeapAlloc(int size);
void HeapFreePtr(void *ptr);
void *HeapRealloc(void *ptr, int size);