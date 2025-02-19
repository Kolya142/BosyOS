#include <libs/mem.h>
#include <kernel/KMem.h>

Ptr MAlloc(U32 size) {
    return HeapAlloc(size);
}
U0 MFree(Ptr ptr) {
    HeapFreePtr(ptr);
}
Ptr MReAlloc(Ptr ptr, U32 olds, U32 news) {
    Ptr new = MAlloc(news);
    MemCpy(new, ptr, olds < news ? olds : news);
    MFree(ptr);
    return new;
}
Ptr MCAlloc(U32 elem, U32 count) {
    Ptr mem = MAlloc(elem * count);
    MemSet(mem, 0, elem * count);
    return mem;
}