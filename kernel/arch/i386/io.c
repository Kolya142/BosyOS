#include <arch/i386/cpu.h>

U0 POut(U16 port, U8 data) {
    __asm__ __volatile__("outb %0, %1" : : "a"(data), "Nd"(port));
}
U8 PIn(U16 port) {
    U8 ret;
    __asm__ __volatile__("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
U0 POut16(U16 port, U16 data) {
    __asm__ __volatile__("outw %0, %1" : : "a"(data), "Nd"(port));
}
U16 PIn16(U16 port) {
    U16 ret;
    __asm__ __volatile__("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
U0 POut32(U16 port, U32 data) {
    __asm__ __volatile__("out %0, %1" : : "a"(data), "Nd"(port));
}
U32 PIn32(U16 port) {
    U32 ret;
    __asm__ __volatile__("in %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
