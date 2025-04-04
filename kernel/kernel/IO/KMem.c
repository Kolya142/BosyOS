#include <kernel/KDogWatch.h>
#include <kernel/KMem.h>
#include <lib/IO/TTY.h>

#define ALIGNMENT (sizeof(HeapMemBlock))

U8 *Heap = (U8*)HEAP_START;
HeapMemBlock *HeapFree = NULL;
U32 HeapUsed = 0;

U0 HeapInit() {
    HeapFree = (HeapMemBlock*)Heap;
    HeapFree->size = HEAP_SIZE;
    HeapFree->ptr = (void *)(Heap + sizeof(HeapMemBlock));
    HeapFree->next = NULL;
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
                curr->orig = size;
            }
            curr->free = False;
            HeapUsed += size;
            // PrintF("Allocated %p bytes at %p\n", size, curr->ptr);
            return curr->ptr;
        }
        curr = curr->next;
    }
    KDogWatchLog("Failed to malloc some memory", False);
    SerialPrintF("Tried to alloc %p bytes\n", size);
    return NULL;
}

U0 HeapFreePtr(Ptr ptr) {
    if (!ptr) return;
    if ((ptr < Heap) || (ptr > Heap + HEAP_SIZE)) return;

    HeapMemBlock *block = (HeapMemBlock *)((U8*)ptr - sizeof(HeapMemBlock));
    HeapUsed -= block->orig;
    // PrintF("freeing block %p with size %p\n", block, block->size);
    block->free = 1;
    
    HeapMemBlock *curr = HeapFree;

    while (curr && curr->next) {
        if (curr->next == block) {
            curr->next = block->next;
            break;
        }
        else if (curr == block) {
            break;
        }
        curr = curr->next;
    }
}
