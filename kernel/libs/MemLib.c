#include <lib/MemLib.h>
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

U0 *MemCpy(U0 *destp, const U0 *src, U32 len) {
    __asm__ volatile (
        "cld\n"
        "rep movsb\n"
        :: "c"(len), "S"(src), "D"(destp)
        : "memory"
    );
    return destp;
}

U0 MemSet(U0 *dest, Byte val, U32 len) {
    __asm__ volatile (
        "cld\n"
        "rep stosb\n"
        :: "a"(val), "D"(dest), "c"(len)
        : "memory"
    );
}
