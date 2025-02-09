#include <kernel/mem.h>

void *MemCpy(void *dest, const void *src, int len)
{
    for (int i = 0; i < len; i++) {
        ((unsigned char*)dest)[i] = ((unsigned char*)src)[i];
    } 
    return dest;
}

void MemSet(void *dest, unsigned char val, int len)
{
    for (int i = 0; i < len; i++) {
        ((unsigned char*)dest)[i] = val;
    }
}