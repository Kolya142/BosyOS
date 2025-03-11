#include <kernel/KDogWatch.h>
#include <kernel/KMem.h>

#define ALIGNMENT (sizeof(HeapMemBlock))

U8 *Heap = (U8*)HEAP_START;
HeapMemBlock *HeapFree = Null;

U0 HeapInit() {
    HeapFree = (HeapMemBlock*)Heap;
    HeapFree->size = HEAP_SIZE;
    HeapFree->ptr = (void *)(Heap + sizeof(HeapMemBlock));
    HeapFree->next = Null;
    HeapFree->free = 1;
}

Ptr HeapAlloc(U32 size) {
    HeapMemBlock *curr = HeapFree;
    U32 aligned = (size + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);
    while (curr) {
        if (curr->free && curr->size >= aligned) {
            if (curr->size >= aligned + sizeof(HeapMemBlock) + ALIGNMENT) {
                U32 curr_addr = (U32)curr->ptr;

                U32 new_addr = (curr_addr + aligned + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);

                U32 over = (new_addr - curr_addr) + sizeof(HeapMemBlock);

                HeapMemBlock *new_block = (HeapMemBlock*)new_addr;
                new_block->size = curr->size - aligned - over;
                new_block->ptr = (U8*)(new_addr + sizeof(HeapMemBlock));
                new_block->free = 1;
                new_block->next = curr->next;
                curr->next = new_block;

                curr->size = aligned;
            }
            curr->free = False;
            return curr->ptr;
        }
        curr = curr->next;
    }
    KDogWatchLog("Failed to malloc some memory", False);
    return Null;
}

U0 HeapFreePtr(Ptr ptr) {
    if (!ptr) return;
    if ((ptr < Heap) || (ptr > Heap + HEAP_SIZE)) return;

    HeapMemBlock *block = (HeapMemBlock *)((U8*)ptr - sizeof(HeapMemBlock));
    block->free = 1;

    HeapMemBlock *heap_free = HeapFree;
    while (heap_free && heap_free->next) {
        if (heap_free->free && heap_free->next->free) {
            heap_free->size += sizeof(HeapMemBlock) + heap_free->next->size;
            heap_free->next = heap_free->next->next;
        } else {
            heap_free = heap_free->next;
        }
    }
}
