#pragma once
#include <lib/String.h>
#include <lib/MemLib.h>
#include <lib/Types.h>
// RamFileSystem
#define RFS_SIZE 512

typedef struct RFSFile {
    Byte *data;
    Bool exists;
    U32 size;
    Char name[8];
} RFSFile;

typedef struct RFSFileDescriptor {
    U32 pos;
    U32 head;
} RFSFileDescriptor;

extern RFSFile *RFS;

U0 RFSInit();
U0 RFSAdd(String name, U32 size);
U0 RFSReSize(String name, U32 size);
U32 RFSOpen(String name);
U32 RFSWrite(U32 fd, Ptr buf, U32 count);
U32 RFSRead(U32 fd, Ptr buf, U32 count);
U0 RFSClose(U32 fd);