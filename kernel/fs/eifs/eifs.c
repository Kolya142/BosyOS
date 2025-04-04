#include <lib/memory/MemLib.h>
#include <drivers/disk/ide.h>
#include <fs/eifs/eifs.h>
#include <lib/IO/TTY.h>
#include <fs/iso9660.h>
static U32 start;
static U32 size;
static EIFSuperBlock sb;

U32 EIFIAlloc() {
    U8 buf[512];
    for (U32 i = sb.iblock_alloc_start; i < sb.iblock_alloc_end; ++i) {
        ATARead(1, buf, start + i, 1);
        for (U32 j = 0; j < 512; ++j) {
            if (buf[j] != 0xFF) {
                for (U32 k = 0; k < 8; ++k) {
                    if (!(buf[j] & (1 << k))) {
                        buf[j] |= 1 << k;
                        ATAWrite(1, buf, start + i, 1);
                        return ((i - sb.iblock_alloc_start) * 512 * 8) + (j * 8) + k;
                    }
                }
            }
        }
    }
    return -1;
}
U32 EIFBAlloc() {
    U8 buf[512];
    for (U32 i = sb.block_alloc_start; i < sb.block_alloc_end; ++i) {
        ATARead(1, buf, start + i, 1);
        for (U32 j = 0; j < 512; ++j) {
            if (buf[j] != 0xFF) {
                for (U32 k = 0; k < 8; ++k) {
                    if (!(buf[j] & (1 << k))) {
                        buf[j] |= 1 << k;
                        ATAWrite(1, buf, start + i, 1);
                        return ((i - sb.block_alloc_start) * 512 * 8) + (j * 8) + k;
                    }
                }
            }
        }
    }
    return -1;
}

EIFINode EIFIGet(U32 i) {
    U8 dbuf[512];
    ATARead(1, dbuf, start + sb.iblock_start + (i / sizeof(EIFINode)) / 512, 1);
    EIFINode ind;
    MemCpy(&ind, dbuf + (i / sizeof(EIFINode)) % 512, sizeof(EIFINode));
    return ind;
}
U0 EIFBGet(U32 i, U8 *buf) {
    ATARead(1, buf, start + sb.block_start + (i / sizeof(EIFINode)) / 512, 1);
}

U0 EIFInit() {
    start = 0;
    size = 4000;
    U8 buf[512];
    ATARead(1, buf, start, 1);
    MemCpy(&sb, buf, sizeof(EIFSuperBlock));
    if (sb.magic != 0x53464945) {
        sb.magic = 0x53464945;
        sb.block_start = 1;
        sb.block_end = 800;
        sb.block_alloc_start = 801;
        sb.block_alloc_end = 1000;

        sb.iblock_start = 1001;
        sb.iblock_end = 1800;
        sb.iblock_alloc_start = 1801;
        sb.iblock_alloc_end = 2000;

        MemCpy(buf, &sb, sizeof(EIFSuperBlock));
        ATAWrite(1, buf, start, 1);
    }
}