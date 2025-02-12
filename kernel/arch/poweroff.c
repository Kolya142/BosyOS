#include <arch/io.h>
#include <arch/cpu.h>

U0 PowerOff() {
    // try 1
    POut(0xB004, 0);
    // try 2
    POut(0x0604, 0);
    // try 3
    for (I32 i = 0; i < 1000; i++) POut(0x0064, 0xFD);
    // try 4
    CpuHalt();
}