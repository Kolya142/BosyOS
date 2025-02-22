#include <lib/MemLib.h>
#include <lib/TTY.h>
#include <misc/bsf.h>
#include <arch/gdt.h>
BsfApp BsfFromBytes(Byte *app) {
    BsfApp bapp;
    MemCpy(&bapp.header, app, sizeof(BsfHeader));
    bapp.data = app + sizeof(BsfHeader);
    return bapp;
}
Bool BsfExec(BsfApp *app) {
    BsfHeader *head = &app->header;
    if (head->Magic != 0x59534F42) { // BOSY
        return False;
    }

    MemCpy((Ptr)0x100000, app->data, head->CodeS);
    // TTYUPrintHex(head->CodeS);
    // TTYUPrint((Ptr)0x100000);

    U0(*entry)() = (U0(*)())0x100000;
    entry();

    // GDTSet(0x100000, 0xFFFFFFFF);
    // GDTLoad();

    // Ptr stack_addr = (Ptr)0x200000 - 4;
    // SYSUserSetup((Ptr)0x100000, stack_addr);

    return True;
}