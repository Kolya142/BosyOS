#include <stdint.h>

uint32_t syscall(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e) {
    uint32_t ret;
    asm volatile(
        "int $0x78\n"
        : "=a"(ret)
        : "a"(a), "S"(b), "D"(c), "b"(d), "d"(e)
        : "memory"
    );
    return ret;
}