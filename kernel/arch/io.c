#include <arch/io.h>

U0 POut(U16 port, U8 data)
{
    __asm__ __volatile__("outb %0, %1" : : "a"(data), "Nd"(port));
}
U8 PIn(U16 port) {
    U8 ret;
    __asm__ __volatile__("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}