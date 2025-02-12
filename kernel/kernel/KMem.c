#include <kernel/KMem.h>

unsigned char *Heap = (void*)HEAP_START + sizeof(HeapMemBlock);
HeapMemBlock *HeapFree = Null;

void HeapInit() {
    HeapFree = (HeapMemBlock*)(Heap - sizeof(HeapMemBlock));
    HeapFree->size = HEAP_SIZE;
    HeapFree->ptr = Heap;
    HeapFree->next = Null;
    HeapFree->free = 1;
} 
void *HeapAlloc(int size) {
    HeapMemBlock *heap_free = HeapFree;
    HeapMemBlock *heap_free_prev = Null;
    while (heap_free) {
        if (heap_free->size == size && heap_free->free) {
            heap_free->free = 0;
            if (heap_free_prev) {
                heap_free_prev->next = Null;
            }
            else {
                HeapFree = heap_free->next;
            }
            return heap_free->ptr;
        }
        if (heap_free->size > size && heap_free->free) {
            /*
                before: [OldBlock][Heap]
                after: [OldBlock][Allocated][NewBlock][Heap]
            */
            HeapMemBlock *new_block = heap_free->ptr + size;
            new_block->size = heap_free->size - size - sizeof(HeapMemBlock);
            new_block->ptr = (void*)new_block + sizeof(HeapMemBlock);
            new_block->next = heap_free->next;
            new_block->free = 1;

            heap_free->next = new_block;
            heap_free->free = 0;
            heap_free->size = size;
            return heap_free->ptr;
        }
        heap_free_prev = heap_free;
        heap_free = heap_free->next;
    }
    return Null;
}

void HeapFreePtr(void *ptr)
{
    if (!ptr) return;
    if ((ptr < Heap) || (ptr > Heap+HEAP_SIZE)) return;

    HeapMemBlock *block = (HeapMemBlock *)((unsigned char *)ptr - sizeof(HeapMemBlock));
    block->free = 1;

    HeapMemBlock *heap_free = HeapFree;
    while (heap_free) {
        if (heap_free->next && heap_free->next->ptr == ptr) {
            heap_free->size += sizeof(HeapMemBlock)+heap_free->next->size;
            heap_free->next = heap_free->next->next;
            continue;
        }
        heap_free = heap_free->next;
    }
}

void *HeapRealloc(void *ptr, int size)
{
    void *nptr = HeapAlloc(size);
    for (int i = 0; i < size; i++) {
        ((unsigned char*)nptr)[i] = ((unsigned char*)ptr)[i];
    }
    HeapFreePtr(ptr);
    return nptr;
}