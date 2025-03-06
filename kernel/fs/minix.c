#include <drivers/ide.h>
#include <lib/String.h>
#include <lib/MemLib.h>
#include <fs/minix.h>
#include <lib/TTY.h>

#define SS 291
#define BLOCK_SIZE 1024

MXSuperBlock MXSB;

Bool MXInodeRead(MXINode *inode, U8 *buf) {
    if ((inode->mode & S_FT) != S_IFREG) {
        return 0;
    }
    if (!(inode->mode & S_IRUSR)) {
        return 0;
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
U32 MXZAlloc() {
    U32 zone_map = (2 + MXSB.imap_blocks) * (BLOCK_SIZE / 512);
    U32 zone_map_sz = MXSB.zmap_blocks;
    U32 zone_map_sz_b = zone_map_sz * BLOCK_SIZE;
    U8 *buf = MAlloc(zone_map_sz_b);
    for (U32 i = 0; i < zone_map_sz * (BLOCK_SIZE / 512); ++i) {
        ATARead(buf + i * 512, SS + zone_map + i, 1);
    }
    U32 sec = 0xFFFFFFFF;
    for (U32 byte = 0; byte < zone_map_sz_b; ++byte) {
        for (U32 bit = 0; bit < 8; ++bit) {
            if (!(buf[byte] & (1 << bit))) {
                buf[byte] |= (1 << bit);
                sec = byte * 8 + bit;
                break;
            }
        }
        if (sec != 0xFFFFFFFF) {
            break;
        }
    }
    for (U32 i = 0; i < zone_map_sz * (BLOCK_SIZE / 512); ++i) {
        ATAWrite(buf + i * 512, SS + zone_map + i, 1);
    }
    MFree(buf);
    return sec;
}
U32 MXIAlloc() {
    U32 imap_map = 2 * (BLOCK_SIZE / 512);
    U32 imap_map_sz = MXSB.imap_blocks;
    U32 imap_map_sz_b = imap_map_sz * BLOCK_SIZE;
    U8 *buf = MAlloc(imap_map_sz_b);
    for (U32 i = 0; i < imap_map_sz * (BLOCK_SIZE / 512); ++i) {
        ATARead(buf + i * 512, SS + imap_map + i, 1);
    }
    U32 sec = 0xFFFFFFFF;
    for (U32 byte = 0; byte < imap_map_sz_b; ++byte) {
        for (U32 bit = 0; bit < 8; ++bit) {
            if (!(buf[byte] & (1 << bit))) {
                buf[byte] |= (1 << bit);
                sec = byte * 8 + bit;
                break;
            }
        }
        if (sec != 0xFFFFFFFF) {
            break;
        }
    }
    for (U32 i = 0; i < imap_map_sz * (BLOCK_SIZE / 512); ++i) {
        ATAWrite(buf + i * 512, SS + imap_map + i, 1);
    }
    MFree(buf);
    return sec;
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
U0 MXInodeSet(MXSuperBlock *sb, U32 index, MXINode node) {
    U32 inode_t_size = (2 + sb->imap_blocks + sb->zmap_blocks) * BLOCK_SIZE;

    U32 inode_offset = inode_t_size + (index - 1) * sizeof(MXINode);

    U8 buf[BLOCK_SIZE];
    
    U32 logical_block = inode_offset / BLOCK_SIZE;
    U32 sector_num = SS + logical_block * (BLOCK_SIZE / 512);
    
    U32 shift = inode_offset % BLOCK_SIZE;

    ATARead(buf, sector_num, BLOCK_SIZE / 512);
    
    MemCpy(buf+shift, &node, sizeof(MXINode));

    ATAWrite(buf, sector_num, BLOCK_SIZE / 512);

    PrintF("Writing inode from sector $!B%4x$!F Disk byte\n", sector_num * 512 + shift);

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
U0 MXCreate(String filename, U16 mode) {
    MXINode parent = MXInodeGet(&MXSB, 1);
    U32 new_inode_num = MXIAlloc();
    if (new_inode_num == 0xFFFFFFFF) {
        return;
    }
    MXINode node;
    node.mode = mode;
    node.size = 0;
    node.uid = 1000;
    node.gid = 271;
    node.time = 0;
    node.nlinks = 0;
    for (U32 i = 0; i < 8; ++i) {
        node.zone[i] = 0;
    }
    MXInodeSet(&MXSB, new_inode_num, node);


    MXINode root = MXInodeGet(&MXSB, 1);
    U8 block[BLOCK_SIZE];
    MXDataGet(root.zone[0], block);
    U32 j = 0;
    for (U32 i = 0; i < BLOCK_SIZE / sizeof(MXDirEntry); ++i) {
        MXDirEntry *entry = (MXDirEntry*)(block + i * sizeof(MXDirEntry));

        if (!entry->inode) {
            entry->inode = new_inode_num;
            StrCpy(entry->name, filename);

            U32 sector = SS + root.zone[0] * (BLOCK_SIZE / 512);
            ATAWrite(block, sector, BLOCK_SIZE / 512);
            break;
        }
    }
}
U32 MXWrite(MXINode *inode, U8 *buf, U32 count) {
    if ((inode->mode & S_FT) != S_IFREG) {
        return 0;
    }
    if (!(inode->mode & S_IWUSR)) {
        return 0;
    }

    U32 blocks = (count + BLOCK_SIZE - 1) / BLOCK_SIZE;

    U32 written = 0;
    U8 temp[BLOCK_SIZE];

    for (U32 i = 0; i < 7 && i < blocks; ++i) {
        if (inode->zone[i] == 0) {
            inode->zone[i] = MXZAlloc();
        }

        U32 twrite = (count - written < BLOCK_SIZE) ? (count - written) : BLOCK_SIZE;
        MemCpy(temp, buf + written, twrite);

        if (twrite < BLOCK_SIZE) {
            MemSet(temp + twrite, 0, BLOCK_SIZE - twrite);
        }

        U32 sector = SS + inode->zone[i] * (BLOCK_SIZE / 512);
        ATAWrite(temp, sector, BLOCK_SIZE / 512);

        written += twrite;
    }

    if (blocks > 7) {
        U16 indirect[BLOCK_SIZE / 2];
        if (inode->zone[7] == 0) {
            inode->zone[7] = MXZAlloc();
            MemSet((U8*)indirect, 0, BLOCK_SIZE);
        }
        else {
            MXDataGet(inode->zone[7], (U8*)indirect);
        }

        for (U32 j = 0; (7 + j) < blocks && j < (BLOCK_SIZE / 2); ++j) {
            if (!indirect[j]) {
                indirect[j] = MXZAlloc();
            }

            U32 twrite = (count - written < BLOCK_SIZE) ? (count - written) : BLOCK_SIZE;
            MemCpy(temp, buf + written, twrite);
            if (twrite < BLOCK_SIZE) {
                MemSet(temp + twrite, 0, BLOCK_SIZE - twrite);
            }
            U32 sector = SS + indirect[j] * (BLOCK_SIZE / 512);
            ATAWrite(temp, sector, BLOCK_SIZE / 512);
            written += twrite;
        }
    }
    inode->size = count;
    return written;
}

U0 MXInit() {
    U8 buf[1024];
    ATARead(buf, SS+2, 2);
    MXSuperBlock *sb = (MXSuperBlock*)buf;
    MemCpy(&MXSB, sb, sizeof(MXSuperBlock));
    if (sb->magic != 0x138f) {
        PrintF("Warning: Invalid minix fs magic ($!B0x%2x$!F)\n", sb->magic);
        return;
    }
    // PrintF("IAlloc test: $!B%x$!F\n", MXIAlloc());
    // PrintF("IAlloc test: $!B%x$!F\n", MXIAlloc());
}