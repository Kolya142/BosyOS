#include <misc/meml.h>
U0 *MemCpy(U0 *destp, const U0 *src, U32 len) {
    U0 *dest = destp;
    while (len) { // TODO: Use x86 rep movsb magic
        *(Byte*)dest = *(Byte*)src;
        dest++;
        src++;
        len--;
    }
    return destp;
}
U0 MemSet(U0 *dest, Byte val, U32 len) {
    while (len) {
        *(Byte*)dest = val;
        dest++;
        len--;
    }
}