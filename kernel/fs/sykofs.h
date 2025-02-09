#include <lib/types.h>
#include <lib/string.h>
#include <drivers/screen.h>
#include <lib/screenlib.h>
#pragma once
#define FSBYTES 1024 * 256
#define FSSECTORS FSBYTES / 512

struct FileDescriptor {
    char Name[32];
    U32 DataSize;
    U32 DataPointer;
};
typedef struct FileDescriptor FileDescriptor;
struct FileSystem {
    FileDescriptor files[64];
    U8 data[FSBYTES];
    U8 allocmap[FSSECTORS / 8];
};
typedef struct FileSystem FileSystem;

void FsInit(FileSystem *fs);
char FsGetFromAllocMap(FileSystem *fs, U32 sectorid);
void FsSetToAllocMap(FileSystem *fs, U32 sectorid, char v);
U32 FsFindFreeSpace(FileSystem *fs, U32 sectors);
void FsFreeSectors(FileSystem *fs, U32 sector, U32 sectors);
FileDescriptor *FsCreate(FileSystem *fs, const char *name);

FileDescriptor *FsRemove(FileSystem *fs, const char *name);