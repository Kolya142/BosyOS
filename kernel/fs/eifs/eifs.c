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
        ATARead(0, buf, start + i, 1);
        for (U32 j = 0; j < 512; ++j) {
            if (buf[j] != 0xFF) {
                for (U32 k = 0; k < 8; ++k) {
                    if (!(buf[j] & (1 << k))) {
                        buf[j] |= 1 << k;
                        ATAWrite(0, buf, start + i, 1);
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
        ATARead(0, buf, start + i, 1);
        for (U32 j = 0; j < 512; ++j) {
            if (buf[j] != 0xFF) {
                for (U32 k = 0; k < 8; ++k) {
                    if (!(buf[j] & (1 << k))) {
                        buf[j] |= 1 << k;
                        ATAWrite(0, buf, start + i, 1);
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
    ATARead(0, dbuf, start + sb.iblock_start + (i / sizeof(EIFINode)) / 512, 1);
    EIFINode ind;
    MemCpy(&ind, dbuf + (i / sizeof(EIFINode)) % 512, sizeof(EIFINode));
    return ind;
}
U0 EIFBGet(U32 i, U8 *buf) {
    ATARead(0, buf, start + sb.block_start + (i / sizeof(EIFINode)) / 512, 1);
}

U0 EIFInit() {
    ISO9660DirEntry *de = ISO9660Get("drive.");
    start = de->extent_lba_le * 4;
    size = (de->data_length_le + 511) / 512;
    U8 buf[512];
    ATARead(0, buf, start, 1);
    MemCpy(&sb, buf, sizeof(EIFSuperBlock));
    if (sb.magic != 'SFIE') {
        sb.magic = 'SFIE';
        sb.block_start = 1;
        sb.block_end = 16;
        sb.block_alloc_start = 17;
        sb.block_alloc_end = 23;

        sb.iblock_start = 24;
        sb.iblock_end = 64;
        sb.iblock_alloc_start = 65;
        sb.iblock_alloc_end = 80;

        MemCpy(buf, &sb, sizeof(EIFSuperBlock));
        ATAWrite(0, buf, start, 1);
    }
    PrintF("IAlloc: %p\n", EIFIAlloc());
}