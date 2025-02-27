#include <lib/MemLib.h>
#include <lib/TTY.h>
#include <misc/bsfexe.h>
#include <arch/gdt.h>
// Bsf-Exe
BsfApp BsfFromBytes(Byte *app) {
    BsfApp bapp;
    MemCpy(&bapp.header, app, sizeof(BsfHeader));
    bapp.data = app + sizeof(BsfHeader);
    return bapp;
}
static volatile U32 esp, ebp;
Bool BsfExec(BsfApp *app) {
    BsfHeader *head = &app->header;
    if (head->Magic != 0x59534F42) { // BOSY
        return False;
    }

    U8 *bossec = app->data + head->CodeS;
    BsfMeta *meta = (BsfMeta*)bossec;
    while (meta->meta1 != 0 && meta->meta2 != 1 && (U32)(meta) < (U32)(head->BosSec+bossec)) {
        meta = &meta[1];
    }
    if (meta->meta1 != 0 || meta->meta2 != 1 || (U32)(meta) >= (U32)(head->BosSec+bossec)) {
        return False;
    }

    MemCpy((Ptr)0x100000, app->data, head->CodeS);
    // TTYUPrintHex(head->CodeS);
    // TTYUPrint((Ptr)0x100000);
    PrintF("%x\n", meta->func);
    asmV(
        "movl %%esp, %1\n"
        "movl %%ebp, %2\n"
        "movl %%esp, %%ebp\n"
        "call *%0\n"
        "movl %2, %%ebp\n"
        "movl %1, %%esp"
    :
    : "r"(meta->func), "m"(esp), "m"(ebp)
    : "memory", "cc");

    // GDTSet(0x100000, 0xFFFFFFFF);
    // GDTLoad();

    // Ptr stack_addr = (Ptr)0x200000 - 4;
    // SYSUserSetup((Ptr)0x100000, stack_addr);

    return True;
}