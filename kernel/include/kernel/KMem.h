#include <lib/MemLib.h>
#include <lib/Types.h>
#pragma once
#define HEAP_START 0x0700000
#define HEAP_SIZE (0x0D00000-0x0700000)

typedef struct HeapMemBlock {
    U32 size;
    Ptr ptr;
    struct HeapMemBlock *next;
    Bool free;
} HeapMemBlock;

extern U8 *Heap; // HEAP_START + sizeof(HeapMemBlock)
extern HeapMemBlock *HeapFree; // NULL
U0 HeapInit();
Ptr HeapAlloc(U32 size);
U0 HeapFreePtr(Ptr ptr);