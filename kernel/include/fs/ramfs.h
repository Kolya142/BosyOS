#pragma once
#include <lib/strings/String.h>
#include <lib/memory/MemLib.h>
#include <lib/Types.h>
#include <fs/vfs.h>
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
U32 RFSReadV(String name, Ptr buf, U32 count);
U32 RFSWriteV(String name, Ptr buf, U32 count);
U32 RFSReadDirV(String, U0(*reader)(String, VFSStat*));