#pragma once
#include <lib/Types.h>
#include <fs/vfs.h>

typedef struct ROFSSB {
    Char magic[7];
    U32 count;
} __attribute__((packed)) ROFSSB;

typedef struct ROFSNode {
    Char name[20];
    U32 size;
} __attribute__((packed)) ROFSNode;

U0 ROFSInit(Byte *buf);