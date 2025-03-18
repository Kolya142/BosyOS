#include <lib/sys/syscall.h>
#include <arch/x86/sys/paging.h>
#include <lib/formats/bsfexe.h>
#include <lib/memory/MemLib.h>
#include <arch/x86/sys/ring3.h>
#include <arch/x86/sys/gdt.h>
#include <lib/IO/TTY.h>
#include <lib/graphics/Graphics.h>

BsfApp BsfFromBytes(Byte *app) {
    BsfApp bapp;
    MemCpy(&bapp.header, app, sizeof(BsfHeader));
    bapp.data = app + sizeof(BsfHeader);
    return bapp;
}
BsfMeta *BsfMetaLoad(Ptr start, U32 d1, U32 d2, U32 size) { // parsing bossec
    BsfMeta *meta = start;
    while ((U32)meta < ((U32)start+size)) {
        if (meta->meta1 == d1 && meta->meta2 == d2) {
            return meta;
        }
        meta = (BsfMeta*)((U8*)meta + sizeof(BsfMeta));
    }
    return meta;
}
#define PAGES 6
#define UADDR 0x0C000000
I32 BsfExec(BsfApp *app, U32 m1, U32 m2) {
    BsfHeader *head = &app->header;
    if (head->Magic != 0x59534F42) { // BOSY
        return False;
    }
    U8 *bossec = app->data + head->CodeS;
    BsfMeta *meta = BsfMetaLoad(bossec, m1, m2, head->BosSec);

    if (!meta || !meta->func) {
        return 0;
    }

    U32 ptrb[PAGES];
    U32 ptr[PAGES];
    for (U32 i = 0; i < PAGES; ++i) {
        ptrb[i] = PGet(UADDR+PAGE_SIZE*i);
        ptr[i] = (U32)PallocMap(UADDR+PAGE_SIZE*i, PAGE_RW | PAGE_USER);
        if (!ptr[i]) {
            for (U32 j = 0; j < i; ++j) {
                PFree((Ptr)ptr[j]);
                PMap(UADDR+PAGE_SIZE*j, ptrb[j], PAGE_RW | PAGE_USER | PAGE_PRESENT);
            }
            return 0;
        }
    }

    MemCpy((Ptr)UADDR, app->data, head->CodeS);
    
    RingSwitch(meta->func, (Ptr)0x300000);

    for (U32 i = 0; i < PAGES; ++i) {
        PFree((Ptr)ptr[i]);
        PMap(UADDR+PAGE_SIZE*i, ptrb[i], PAGE_RW | PAGE_USER | PAGE_PRESENT);
    }

    return 2;
}