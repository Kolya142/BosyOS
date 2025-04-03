#include <lib/memory/MemLib.h>
#include <lib/Types.h>
#pragma once
#define HEAP_START 0x1300000
#define HEAP_SIZE (0x5000000-0x1300000)

typedef struct HeapMemBlock {
    U32 size;
    U32 orig;
    Ptr ptr;
    struct HeapMemBlock *next;
    Bool free;
} HeapMemBlock;

extern U8 *Heap; // HEAP_START + sizeof(HeapMemBlock)
extern HeapMemBlock *HeapFree; // NULL
extern U32 HeapUsed; // 0
U0 HeapInit();
Ptr HeapAlloc(U32 size);
U0 HeapFreePtr(Ptr ptr);