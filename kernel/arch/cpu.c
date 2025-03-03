#include <arch/cpu.h>
#include <arch/io.h>

U0 CpuNameGet(String name) { // original https://github.com/ArTicZera/NovaOS/blob/main/Hardware/cpu.c#L37
    U32 eax, ebx, ecx, edx;

    for (U8 i = 0; i < 3; ++i) {
        eax = 0x80000002 + i;
        asmv (
            "cpuid"
            : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
            : "a" (eax)
        );

        *(U32*)(name + i * 16 + 0) = eax;
        *(U32*)(name + i * 16 + 4) = ebx;
        *(U32*)(name + i * 16 + 8) = ecx;
        *(U32*)(name + i * 16 + 12) = edx;
    }
    name[48] = 0;
}

U16 MemorySize() {
    U8 low, high;
    POut(0x70, 0x30);
    low = PIn(0x71);
    POut(0x70, 0x31);
    high = PIn(0x71);
    return low | (high << 8);
}