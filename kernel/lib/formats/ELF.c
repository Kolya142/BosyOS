#include <arch/x86/sys/paging.h>
#include <lib/formats/ELF.h>
#include <kernel/KTasks.h>
#include <lib/IO/TTY.h>

U32 ELFLoad(U8 *buf)
{
    ELFHeader *elf = (ELFHeader*)buf;

    if (elf->magic != 0x464C457F) {
        PrintF("Invalid ELF Magic: %x", elf->magic);
        return 1;
    }
    
    if (elf->arch != 1 || elf->machine != 3) {
        return 2;
    }

    ELFProgramHeader *ph = (ELFProgramHeader*)((U32)buf + elf->phoff);
    TaskNew(elf->entry, 0x23, 0x1B);
    Task *task = TaskLast;

    U32 page = 0;

    for (U32 i = 0; i < elf->phnum; ++i, ++ph) {
        if (ph->type != 1) {
            continue;
        }

        SerialPrintF("ELF Segment: type=%d vadddr=%X, memsz=%X, filesz=%X", ph->type, ph->vaddr, ph->memsz, ph->filesz);

        for (U32 off = 0; off < ph->memsz; off += 0x1000) {
            Ptr raddr = PAlloc();
            U32 page_off = ph->vaddr + off;
            
            if (off < ph->filesz) {
                U32 copy_len = min(0x1000, ph->filesz - off);
                MemCpy(raddr, buf + ph->offset + off, copy_len);
                
                if (copy_len < 0x1000 && ph->memsz > ph->filesz) {
                    MemSet(raddr + copy_len, 0, 0x1000 - copy_len);
                }
            }
            else {
                MemSet(raddr, 0, 0x1000);
            }

            task->pages[page].vaddr = page_off;
            task->pages[page].raddr = (U32)raddr;
            task->pages[page].exists = True;
            ++page;
            SerialPrintF("Page: vaddr=%p, raddr=%p", page_off, raddr);
        }
    }
    MFree((Ptr)task->esp); // Removing STST(Standart Task STack)

    Ptr stack = PAlloc();
    task->pages[page].vaddr = 0x0C000000;
    task->pages[page].raddr = (U32)stack;
    task->pages[page].exists = True;
    // for(;;);
    return 0;
}