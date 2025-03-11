#include <lib/formats/ELF.h>
#include <lib/IO/TTY.h>

I32 ELFLoad(U8 *buf)
{
    ELFHeader *elf = (ELFHeader*)buf;

    if (elf->magic != 0x464C457F) {
        SerialPrintF("ELF Magic: %x", elf->magic);
        return -1;
    }
    
    if (elf->arch != 1 || elf->machine != 3) {
        return -2;
    }

    SerialPrintF("ELF Entry: %x\n", elf->entry);
}