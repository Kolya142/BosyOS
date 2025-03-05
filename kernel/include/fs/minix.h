#pragma once
#include <lib/Types.h>

#define S_IFSOCK 0xC000
#define S_IFLINK 0xA000
#define S_IFREG  0x8000
#define S_IFBLK  0x6000
#define S_IFDIR  0x4000
#define S_IFCHR  0x2000
#define S_IFIFO  0x1000

#define S_ISUID  0x0800
#define S_ISGID  0x0400
#define S_ISVTX  0x0200

#define S_IRUSR  0x0100
#define S_IWUSR  0x0080
#define S_IXUSR  0x0040

#define S_IRGRP  0x0020
#define S_IWGRP  0x0010
#define S_IXGRP  0x0008

#define S_IROUT  0x0004
#define S_IWOUT  0x0002
#define S_IXOUT  0x0001

typedef struct MXDirEntry {
    U16 inode;
    Char name[14];
} __attribute__((packed)) MXDirEntry;

typedef struct MXSuperBlock {
    U16 ninodes;
    U16 nzones;
    U16 imap_blocks;
    U16 zmap_blocks;
    U16 first_data_zone;
    U16 log_zone_size;

    U32 max_size;
    U16 magic;
    U16 state;
} __attribute__((packed)) MXSuperBlock;

typedef struct MXINode {
    U16 mode;
    U16 uid;
    U32 size;
    U32 time;
    U8  gid;
    U8  nlinks;
    U16 zone[9];
} __attribute__((packed)) MXINode;

extern MXSuperBlock MXSB;

Bool MXInodeDataRead(MXINode *inode, U8 *buf);
MXINode MXInodeGet(MXSuperBlock *sb, U32 index);
U8 *MXDataGet(U32 index, U8 *block);
U0 MXListRoot(MXDirEntry *arr, U32 count);

U0 MXInit();