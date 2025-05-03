#include <kernel/KHeap.h>

typedef struct KHeapBlock {
    U32 size;
    Bool free;
    struct KHeapBlock *next;
} KHeapBlock;

static KHeapBlock *start;

U0 KHeapInit() {
    start = (Ptr)KERNEL_HEAP_START;
    start->size = KERNEL_HEAP_END - KERNEL_HEAP_START - sizeof(KHeapBlock);
    start->next = NULL;
    start->free = 1;
}

Bool KHeapSanityCheck() {
    KHeapBlock *curr = start;
    while (curr) {
        if ((U32)curr < KERNEL_HEAP_START || (U32)curr > KERNEL_HEAP_END) {
            return True;
        }
        curr = curr->next;
    }
    return False;
}

Ptr KMAlloc(U32 count) {
    KHeapBlock *curr = start;
    while (curr) {
        if (curr->free) {
            if (curr->size == count) {
                return ((Ptr)curr) + sizeof(KHeapBlock);
            }
            else if (curr->size >= count + sizeof(KHeapBlock)) {
                KHeapBlock *new = ((Ptr)curr) + sizeof(KHeapBlock) + count;
            
                new->size = curr->size - count - sizeof(KHeapBlock);
                new->free = 1;
                new->next = curr->next;
            
                curr->size = count;
                curr->free = 0;
                curr->next = new;
            
                return ((Ptr)curr) + sizeof(KHeapBlock);
            }      
            else if (curr->size > count) {
                curr->free = 0;
                return ((Ptr)curr) + sizeof(KHeapBlock);
            }
        }
        curr = curr->next;
    }
    return NULL;
}

U0 KMFree(Ptr ptr) {
    if ((U32)ptr < KERNEL_HEAP_START || (U32)ptr > KERNEL_HEAP_END) {
        return;
    }
    KHeapBlock *curr = start;
    KHeapBlock *block = ptr - sizeof(KHeapBlock);
    while (curr && curr->next) {
        if (curr->next == block) {
            block->free = 1;
        }
        if (curr->free && curr->next->free) {
            curr->size += curr->next->size + sizeof(KHeapBlock);
            curr->next = curr->next->next;
            continue;
        }
        curr = curr->next;
    }
}
