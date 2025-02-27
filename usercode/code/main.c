#include "bosyos/bsfexe.h"
#include "bosystd.h"

void main() {
    syscall(1, (uint32_t)"Hello, world!\n", 0, 0, 0);
    syscall(2, syscall(0xb, 0, 0, 0, 0), 0, 0, 0);
    for (uint32_t i = 20; i < 40; ++i) {
        syscall(8, i, 30, 0, 0);
        syscall(4, i, 0, 0, 0);
    }
}

FUNCTION(main, 0, 1)