#include <kernel/KMem.h>

unsigned char *Heap = (void*)HEAP_START;
HeapMemBlock *HeapFree = Null;

void HeapInit() {
    HeapFree = (HeapMemBlock*)Heap;
    HeapFree->size = HEAP_SIZE;
    HeapFree->ptr = (void *)(Heap + sizeof(HeapMemBlock));
    HeapFree->next = Null;
    HeapFree->free = 1;
}

void *HeapAlloc(int size) {
    HeapMemBlock *heap_free = HeapFree;

    while (heap_free) {
        if (heap_free->size >= size + sizeof(HeapMemBlock) && heap_free->free) {
            HeapMemBlock *new_block = (HeapMemBlock *)((U32)heap_free->ptr + size);
            new_block = (HeapMemBlock *)(((U32)new_block + sizeof(HeapMemBlock) - 1) & ~(sizeof(HeapMemBlock) - 1));

            new_block->size = heap_free->size - size - sizeof(HeapMemBlock);
            new_block->ptr = (void *)((U32)new_block + sizeof(HeapMemBlock));
            new_block->free = 1;
            new_block->next = heap_free->next;

            heap_free->next = new_block;
            heap_free->size = size;
            heap_free->free = 0;

            return heap_free->ptr;
        }

        heap_free = heap_free->next;
    }

    return Null;
}

void HeapFreePtr(void *ptr) {
    if (!ptr) return;
    if ((ptr < Heap) || (ptr > Heap + HEAP_SIZE)) return;

    HeapMemBlock *block = (HeapMemBlock *)((unsigned char *)ptr - sizeof(HeapMemBlock));
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
