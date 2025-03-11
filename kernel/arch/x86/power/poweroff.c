#include <arch/x86/cpu/io.h>
#include <arch/x86/cpu/cpu.h>

U0 PowerOff() {
    // try 0
    POut(0x0604, 0x2000);
    // try 1
    POut(0xB004, 0);
    // try 2
    POut(0x0604, 0);
    // try 3
    for (I32 i = 0; i < 1000; i++) POut(0x0064, 0xFD);
    // try 4
    CpuHalt();
}