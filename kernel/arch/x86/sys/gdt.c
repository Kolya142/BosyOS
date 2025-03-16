#include <lib/memory/MemLib.h>
#include <lib/IO/TTY.h>
#include <arch/x86/sys/gdt.h>

TTSt TSS;
GDTEntry GDTEntries[6];
GDTPointer GDTPtr;

U0 GDTInit() {
    GDTPtr.size = (sizeof(GDTEntry) * 6) - 1;
    GDTPtr.addr = (U32)&GDTEntries;
    MemSet(&TSS, 0, sizeof(TSS));

    GDTEntrySet(0, 0, 0,                   0,    0   );      // Null Segment (0x00) ---
    GDTEntrySet(1, 0, 0xFFFFFFFF,          0x9A, 0xCF);      // Kernel Code  (0x08) r-x
    GDTEntrySet(2, 0, 0xFFFFFFFF,          0x92, 0xCF);      // Kernel Data  (0x10) rw-
    GDTEntrySet(3, 0, 0xFFFFFFFF,          0xFA, 0xCF);      // User Code    (0x18) r-x
    GDTEntrySet(4, 0, 0xFFFFFFFF,          0xF2, 0xCF);      // User Data    (0x20) rw-
    GDTEntrySet(5, (U32)&TSS, sizeof(TSS), 0x89, 0x40);      // TSS          (0x28) --x
    TSS.esp0 = 0x600000;
    TSS.ss0  = 0x10;
    GDTLoad();
    asmV(
        "movw $0x28, %%ax\n"
        "ltr %%ax\n"
        ::: "eax"
    );
}

U0 GDTLoad() {
    asmV(
        "lgdt (%0)\n"
        "movw $0x10, %%ax\n"
        "movw %%ax, %%ds\n"
        "movw %%ax, %%es\n"
        "movw $0,   %%ax\n"
        "movw %%ax, %%fs\n"
        "movw %%ax, %%gs\n"
        "jmpl $0x08, $.CSReload\n" // Replace with "jmpf" if your system doen't support jmp flags
        ".CSReload:\n"
        :: "m"(GDTPtr)
        : "eax"
    );
}
U0 GDTSet(U32 base, U32 limit) {
    GDTEntrySet(3, base, limit, 0xFA, 0xCF);
    GDTEntrySet(4, base, limit, 0xF2, 0xCF);
}
U0 GDTEntrySet(U8 index, U32 base, U32 limit, U8 access, U8 len) {
    GDTEntries[index].base_low = (base & 0xFFFF);
    GDTEntries[index].base_middle = (base >> 16) & 0xFF;
    GDTEntries[index].base_high = (base >> 24) & 0xFF;
    GDTEntries[index].lim_low = (limit & 0xFFFF);
    GDTEntries[index].length = ((limit >> 16) & 0x0F) | (len & 0xF0);
    GDTEntries[index].access = access;
}

