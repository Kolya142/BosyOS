#include <kernel/KDogWatch.h>
#include <kernel/KPanic.h>
#include <kernel/KMem.h>
#include <lib/IO/TTY.h>

#define ALIGNMENT (sizeof(HeapMemBlock))

U8 *Heap = (U8*)HEAP_START;
HeapMemBlock *HeapFree = NULL;
U32 HeapUsed = 0;

U0 HeapInit() {
    HeapFree = (HeapMemBlock*)Heap;
    HeapFree->size = HEAP_SIZE;
    HeapFree->ptr = (Ptr)(Heap + sizeof(HeapMemBlock));
    HeapFree->next = NULL;
    HeapFree->free = 1;
}

U0 HeapSanityCheck() {
    HeapMemBlock *curr = HeapFree;
    while (curr) {
        if ((U32)curr < (U32)Heap || (U32)curr > (U32)(Heap + HEAP_SIZE)) {
            SerialPrintF("Heap corrupt at %p\n", curr);
            KPanic("Heap corrupt", False);
        }
        curr = curr->next;
    }
}

Ptr HeapAlloc(U32 size) {
    HeapMemBlock *curr = HeapFree;
    U32 aligned = (size + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);
    while (curr) {
        if (curr->free && curr->size >= aligned) {
            U32 curr_addr = (U32)curr->ptr;
            U32 new_addr = (curr_addr + aligned + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);
            U32 over = (new_addr - curr_addr) + sizeof(HeapMemBlock);
            if (curr->size >= aligned + over) {
                HeapMemBlock *new_block = (HeapMemBlock*)new_addr;
                new_block->size = curr->size - aligned - over;
                new_block->ptr = (U8*)new_block + sizeof(HeapMemBlock);
                new_block->free = 1;
                new_block->next = curr->next;

                curr->next = new_block;
                curr->size = aligned;
            }
            curr->free = 0;
            HeapUsed += curr->size;

            // SerialPrintF("Allocated %p bytes at %p\n", size, curr->ptr);
            HeapSanityCheck();
            return curr->ptr;
        }
        curr = curr->next;
    }
    SerialPrintF("Failed to alloc %p bytes. Allocated %p bytes\n", size, HeapUsed);
    KDogWatchLog("Failed to malloc some memory", False);
    HeapSanityCheck();
    return NULL;
}

U0 HeapFreePtr(Ptr ptr) {
    if (!ptr) return;
    if ((ptr < Heap) || (ptr > Heap + HEAP_SIZE)) return;

    HeapMemBlock *block = (HeapMemBlock *)((U8*)ptr - sizeof(HeapMemBlock));
    if (block->free) return;

    HeapUsed -= block->size;
    // PrintF("freeing block %p with size %p\n", block, block->size);
    block->free = 1;

    HeapMemBlock *curr = HeapFree;

    // if (block != HeapFree) {
    //     block->next = HeapFree;
    //     HeapFree = block;
    // }

    while (curr && curr->next) {
        if (curr->free && curr->next->free) {
            curr->size += curr->next->size + sizeof(HeapMemBlock);
            curr->next = curr->next->next;
        }
        else {
            curr = curr->next;
        }
    }
    // curr = HeapFree;
    // while (curr) {
    //     SerialPrintF("%s block with size %p at %p\n", curr->free ? "Free" : "Used", curr->size, curr);
    //     curr = curr->next;
    // }
    HeapSanityCheck();
}
