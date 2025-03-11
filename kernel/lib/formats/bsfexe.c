#include <lib/sys/syscall.h>
#include <arch/x86/sys/paging.h>
#include <lib/formats/bsfexe.h>
#include <lib/memory/MemLib.h>
#include <arch/x86/sys/ring3.h>
#include <arch/x86/sys/gdt.h>
#include <lib/IO/TTY.h>

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
#define PAGES 6
#define UADDR 0x2000000
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
    U32 ptrb[PAGES];
    U32 ptr[PAGES];
    for (U32 i = 0; i < PAGES; ++i) {
        ptrb[i] = PGet(UADDR+PAGE_SIZE*i);
        ptr[i] = (U32)PallocMap(UADDR+PAGE_SIZE*i, PAGE_RW | PAGE_USER);
    }
    asmV( // Save state
        "int $0x78\n"
        : "=a"(s), "=b"(code)
        : "a"(0x14), "S"(2)
    );
    if (s) {
        for (U32 i = 0; i < PAGES; ++i) {
            PFree((Ptr)ptr[i]);
            PMap(UADDR+PAGE_SIZE, ptrb[i], PAGE_RW | PAGE_USER | PAGE_PRESENT);
        }
        return code+2;
    }
    MemCpy((Ptr)UADDR, app->data, head->CodeS);
    // TTYUPrintHex((U32)meta->func);
    // TTYUPrint((Ptr)0x100000);
    PrintF("%p\n", meta->func);
    // RingSwitch((Ptr)meta->func, (Ptr)UADDR+PAGE_SIZE*5 - 4);
    asmV( // Ring0
        "movl %%esp, %1\n"
        "movl %%ebp, %2\n"
        "movl $0x2005000, %%esp\n" // 0x2000000+4096*5
        "movl %%esp, %%ebp\n"
        "jmp *%0\n"
        "movl %2, %%ebp\n"
        "movl %1, %%esp"
    :
    : "r"(meta->func), "m"(esp), "m"(ebp)
    : "memory", "cc");

    // GDTSet(0x100000, 0xFFFFFFFF);
    // GDTLoad();

    return True;
}