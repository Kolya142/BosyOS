#include <misc/meml.h>
#include <user/bsf.h>
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
    GDTSet((U32)app->data, head->CodeS);

    SYSUserSetup(app->data, (Ptr)0);

    return True;
}