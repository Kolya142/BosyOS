#include "ports.h"

void outb(U16 port, U8 val) {
    Asm("outb %0, %1" : : "a"(val), "Nd"(port));
}
U8 inb(U16 port) {
    U8 ret;
    Asm("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}