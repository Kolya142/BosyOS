#pragma once
#include <lib/Types.h>

typedef struct BFSHeader {
    Char drive;
    U32 size;
    U32 datashift;
} __attribute__((packed)) BFSHeader;

typedef struct BFSFile {
    Char name[8];
    U32 size;
    U32 sector;
    U8 flags;
} __attribute__((packed)) BFSFile;

extern BFSHeader drives[5];

BFSHeader *VFSNew(U32 filecount, U32 datalength);
BFSFile *FileFind(Char drive, const String name);
U0 FileCreate(Char drive, const String name);