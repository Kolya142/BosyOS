#include "bosyos/bsfexe.h"

uint32_t syscall(uint32_t a, uint32_t b, uint32_t c) {
    uint32_t ret;
    asm volatile(
        "int 0x80\n"
        : "=a"(ret)
        : "a"(a), "S"(b), "D"(c)
        : "memory"
    );
    return ret;
}

void main() {
    syscall(1, (uint32_t)"Hello, world!", 0);
}

FUNCTION(main, 0, 1)