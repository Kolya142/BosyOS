#include <arch/paging.h>
#include <misc/bsfexe.h>
#include <lib/MemLib.h>
#include <arch/ring3.h>
#include <arch/gdt.h>
#include <lib/TTY.h>

BsfApp BsfFromBytes(Byte *app) {
    BsfApp bapp;
    MemCpy(&bapp.header, app, sizeof(BsfHeader));
    bapp.data = app + sizeof(BsfHeader);
    return bapp;
}
BsfMeta *BsfMetaLoad(Ptr start, U32 d1, U32 d2, U32 size) { // parsing bossec
    BsfMeta *meta = start;
    while ((meta->meta1 != d1 || meta->meta2 != d2) && (U32)(meta) < (U32)(size+start)) {
        meta = &meta[1];
    }
    if (meta->meta1 != d1 || meta->meta2 != d2 || (U32)(meta) >= (U32)(size+start)) {
        return Null;
    }
    return meta;
}
static volatile U32 esp, ebp;
I32 BsfExec(BsfApp *app, U32 m1, U32 m2) {
    BsfHeader *head = &app->header;
    if (head->Magic != 0x59534F42) { // BOSY
        return False;
    }

    U8 *bossec = app->data + head->CodeS;
    BsfMeta *meta = BsfMetaLoad(bossec, m1, m2, head->BosSec);
    Bool s;
    I32 code;
    asmV( // Save state
        "int $0x78\n"
        : "=a"(s), "=b"(code)
        : "a"(0x14), "S"(2)
    );
    if (s) {
        return code+2;
    }
    U32 ptrb = PGet(0x2000000);
    U32 ptrb2 = PGet(0x2000000+PAGE_SIZE);
    Ptr ptr = PallocMap(0x2000000, PAGE_RW | PAGE_USER);
    Ptr ptr2 = PallocMap(0x2000000+PAGE_SIZE, PAGE_RW | PAGE_USER);
    MemCpy((Ptr)0x2000000, app->data, head->CodeS);
    // TTYUPrintHex((U32)meta->func);
    // TTYUPrint((Ptr)0x100000);
    PrintF("%p\n", meta->func);
    // RingSwitch((Ptr)meta->func, (Ptr)0x200000 - 4);
    asmV(
        "movl %%esp, %1\n"
        "movl %%ebp, %2\n"
        "movl %%esp, %%ebp\n"
        "jmp *%0\n"
        "movl %2, %%ebp\n"
        "movl %1, %%esp"
    :
    : "r"(meta->func), "m"(esp), "m"(ebp)
    : "memory", "cc");

    // GDTSet(0x100000, 0xFFFFFFFF);
    // GDTLoad();

    // Ptr stack_addr = (Ptr)0x200000 - 4;
    // SYSUserSetup((Ptr)0x100000, stack_addr);
    PFree(ptr);
    PFree(ptr2);
    PMap(0x2000000, ptrb, PAGE_RW | PAGE_USER | PAGE_PRESENT);
    PMap(0x2000000+PAGE_SIZE, ptrb2, PAGE_RW | PAGE_USER | PAGE_PRESENT);
    return True;
}