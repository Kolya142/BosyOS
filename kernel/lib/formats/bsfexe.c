#include <lib/sys/syscall.h>
#include <arch/x86/sys/paging.h>
#include <lib/formats/bsfexe.h>
#include <lib/memory/MemLib.h>
#include <arch/x86/sys/ring3.h>
#include <arch/x86/sys/gdt.h>
#include <lib/IO/TTY.h>
#include <lib/graphics/Graphics.h>
#include <kernel/KTasks.h>
#include <kernel/KPanic.h>
#include <fs/vfs.h>

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

    U32 tid = TaskNew(UADDR, 0x23, 0x1B);
    Task *task = TaskLast;
    MFree((Ptr)task->esp); // Remove STSP(Standart Task Stack Pointer)
    task->regs.esp = UADDR + (PAGES - 1) * PAGE_SIZE;

    for (U32 i = 0; i < PAGES; ++i) {
        U32 addr = (U32)PAlloc();
        if (!addr) {
            KPanic("No free space for program", False); // TODO: make it safer
        }
        MemCpy((Ptr)addr, (Ptr)(app->data + i * PAGE_SIZE), PAGE_SIZE);
        task->pages[i].exists = True;
        task->pages[i].vaddr = UADDR + i * PAGE_SIZE;
        task->pages[i].raddr = addr;
    }

    return 2;
}