#include <kernel/KDogWatch.h>
#include <lib/MemLib.h>
#include <lib/DAT.h>
#include <lib/TTY.h>
// Disk Allocation Table

/*
Disk Layout:
Boot  DATHeader UserData Kernel
 |     |         |         |
 |    [DAT  Tables]        |
[2]   [1]       [7]      [...]
*/

#define DAT_FIRST_SEG 3

DATHeader DATHeaderGlob;

DATHeader *DATInit() {
    U8 sector[512] = {0};
    ATARead(sector, 3, 1);
    MemCpy(&DATHeaderGlob, sector, sizeof(DATHeader));
    if (DATHeaderGlob.magic != 0x4442) {
        MemSet(&DATHeaderGlob, 0, sizeof(DATHeader));
        DATHeaderGlob.magic = 0x4442;
        DATFlush();
        KDogWatchLog("DAT Created", False);
    }
    else {
        KDogWatchLog("DAT Loaded", False);
    }
    TTYUPrint("DAT Size: ");
    TTYUPrintDec(DAT_SECTORS);
    TTYUPrintC('\n');
    return &DATHeaderGlob;
}

U0 DATFlush() {
    U8 sector[512];
    MemSet(sector, 0, 512);
    MemCpy(sector, &DATHeaderGlob, sizeof(DATHeaderGlob));
    ATAWrite(sector, DAT_FIRST_SEG, 1);
}
U16 DATFind(U16 id) {
    for (U32 i = 1; i < sizeof(DATHeaderGlob.id_table)/2; ++i) {
        if (DATHeaderGlob.id_table[i] == id) {
            return i;
        }
    }
    return 0xFFFF;
}

U32 DATAlloc(U16 id) {
    U16 sec = DATFind(id);
    if (sec != 0xFFFF) return sec;
    for (U32 i = 0; i < sizeof(DATHeaderGlob.alloc_table); ++i) {
        for (U8 j = 0; j < 8; ++j) {
            U32 k = i * 8 + j;
            if (k == 0) {
                continue;
            }
            if (!(DATHeaderGlob.alloc_table[i] & (1 << j))) {
                DATHeaderGlob.alloc_table[i] |= 1 << j;
                DATHeaderGlob.id_table[k] = id;
                DATFlush();
                return k;
            }
        }
    }
    return 0xFFFFFFFF;
}
U0 DATFree(U16 id) {
    for (U32 i = 0; i < sizeof(DATHeaderGlob.alloc_table) / sizeof(U16); ++i) {
        for (U8 j = 0; j < 8; ++j) {
            U32 k = i * 8 + j;
            if (k == 0) continue;
            if (DATHeaderGlob.id_table[k] == id) {
                DATHeaderGlob.alloc_table[i] &= ~(1 << j);
                DATHeaderGlob.id_table[k] = 0;
                DATFlush();
                return;
            }
        }
    }
}

U0 DATWrite(U16 id, Ptr buf, U16 start, U16 count) {
    if (start + count > 512) return;
    U16 sec = DATFind(id);
    if (sec == 0xFFFF) return;

    U8 sector[512];
    ATARead(sector, DAT_FIRST_SEG+sec, 1);
    MemCpy(sector+start, buf, count);
    ATAWrite(sector, DAT_FIRST_SEG+sec, 1);
    ATARead(sector, DAT_FIRST_SEG + sec, 1);
}
U0 DATRead(U16 id, Ptr buf, U16 start, U16 count) {
    if (start + count > 512) return; 
    U16 sec = DATFind(id);
    if (sec == 0xFFFF) return;

    U8 sector[512];
    ATARead(sector, DAT_FIRST_SEG+sec, 1);
    MemCpy(buf, sector+start, count);
}