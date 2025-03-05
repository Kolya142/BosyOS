#include <drivers/ide.h>
#include <lib/String.h>
#include <lib/MemLib.h>
#include <fs/minix.h>
#include <lib/TTY.h>

#define SS 291
#define BLOCK_SIZE 1024

MXSuperBlock MXSB;

Bool MXInodeDataRead(MXINode *inode, U8 *buf) {
    if ((inode->mode & 0xF000) != S_IFREG) {
        return False;
    }

    U32 file_size = inode->size;
    U32 num_blocks = (file_size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    
    U32 i;

    for (i = 0; i < 7 && i < num_blocks; ++i) {
        if (inode->zone[i] == 0) {
            break;
        }
        MXDataGet(inode->zone[i], buf + i * BLOCK_SIZE);
    }
    if (num_blocks > 7 && inode->zone[7] != 0) {
        U16 indirect_blocks[BLOCK_SIZE / 2];
        MXDataGet(inode->zone[7], (U8*)indirect_blocks);

        for (U32 j = 0; i < num_blocks && j < BLOCK_SIZE / 2; ++j, ++i) {
            if (!indirect_blocks[j]) break;

            MXDataGet(indirect_blocks[j], buf + i * BLOCK_SIZE);
        }
    }
    return True;
}

MXINode MXInodeGet(MXSuperBlock *sb, U32 index) {
    U32 inode_t_size = (2 + sb->imap_blocks + sb->zmap_blocks) * BLOCK_SIZE;

    U32 inode_offset = inode_t_size + (index - 1) * sizeof(MXINode);

    MXINode node;
    U8 buf[BLOCK_SIZE];

    U32 logical_block = inode_offset / BLOCK_SIZE;
    U32 sector_num = SS + logical_block * (BLOCK_SIZE / 512);

    U32 shift = inode_offset % BLOCK_SIZE;
    
    ATARead(buf, sector_num, BLOCK_SIZE / 512);

    PrintF("Reading inode from sector $!B%4x$!F Disk byte\n", sector_num * 512 + shift);

    MemCpy(&node, buf+shift, sizeof(MXINode));
    return node;
}
U8 *MXDataGet(U32 index, U8 *block) {
    U32 sector = SS + index * (BLOCK_SIZE / 512);
    ATARead(block, sector, BLOCK_SIZE / 512);
    return block;
}
U0 MXListRoot(MXDirEntry *arr, U32 count) {
    MXINode root = MXInodeGet(&MXSB, 1);
    U8 block[BLOCK_SIZE];
    MXDataGet(root.zone[0], block);
    U32 j = 0;
    for (U32 i = 0; i < BLOCK_SIZE / sizeof(MXDirEntry); ++i) {
        MXDirEntry *entry = (MXDirEntry*)(block + i * sizeof(MXDirEntry));

        if (!entry->inode) {
            continue;
        }
        
        MemCpy(&arr[j++], entry, sizeof(MXDirEntry));

        if (j == count) {
            break;
        }
    }
}

U0 MXInit() {
    U8 buf[1024];
    ATARead(buf, SS+2, 2);
    MXSuperBlock *sb = (MXSuperBlock*)buf;
    if (sb->magic != 0x138f) {
        PrintF("Warning: Invalid minix fs magic ($!B0x%2x$!F)\n", sb->magic);
        return;
    }
    MemCpy(&MXSB, sb, sizeof(MXSuperBlock));
}