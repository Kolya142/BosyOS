#include "sykofs.h"

void FsInit(FileSystem *fs)
{
    for (int f = 0; f < 64; f++) {
        TTYPrint("Fill file(to 0x3f): ");
        PrintHex(f);
        TTYPrintChar('\n');
        TTYCursor -= 80;
        for (int n = 0; n < 32; n++) {
            fs->files[f].Name[n] = 0;
            fs->files[f].DataSize = 0;
            fs->files[f].DataPointer = 0;
        }
    }
    for (int i = 0; i < FSBYTES; i++) {
        if (i%128 == 0) {
            TTYPrint("Fill byte(to 0x40000): ");
            PrintHex(i);
            TTYPrintChar('\n');
            TTYCursor -= 80;
        }
        fs->data[i] = 0;
        // if (i%512 == 0) {
        //     fs->allocmap[i/512/8] = 0;
        // }
    }
}

char FsGetFromAllocMap(FileSystem *fs, U32 sectorid)
{
    U32 over = sectorid/8;
    U32 shift = sectorid%8;
    return (fs->allocmap[over]>>shift) & 1;
}

void FsSetToAllocMap(FileSystem *fs, U32 sectorid, char v)
{
    U32 over = sectorid/8;
    U32 shift = sectorid%8;
    if (v) {
        fs->allocmap[over] |= (1 << shift);
    }
    else {
        fs->allocmap[over] &= !(1 << shift);
    }
}

U32 FsFindFreeSpace(FileSystem *fs, U32 sectors)
{
    for (U32 i = 0; i < FSSECTORS; i++) {
        if (!FsGetFromAllocMap(fs, i)) {
            char freed = 1;
            for (U32 j = i+1; j < i + sectors; j++) {
                if (!FsGetFromAllocMap(fs, j)) {
                    freed = 0;
                    continue;
                }
            }
            if (freed) {
                for (U32 j = i; j < i + sectors; j++) {
                    FsSetToAllocMap(fs, j, 1);
                }
                return i;
            }
        }
    }
    return 0xffffffff;
}

void FsFreeSectors(FileSystem *fs, U32 sector, U32 sectors)
{
    for (U32 i = sector; i < sector + sectors; i++) {
        FsSetToAllocMap(fs, i, 0);
    }
}

FileDescriptor *FsCreate(FileSystem *fs, const char *name) {
    for (U32 i = 0; i < 64; i++) {
        if (!fs->files[i].DataPointer) {
            for (int j = 0; j < 32; j++) {
                fs->files[i].Name[j] = 0;
            }
            for (int j = 0; name[j] && j < 32; j++) {
                fs->files[i].Name[j] = name[j];
            }
            U32 sector = FsFindFreeSpace(fs, 2);
            if (sector == 0xffffffff) {
                return 0;
            }
            fs->files[i].DataPointer = sector*512;
            fs->files[i].DataSize = 1024;
            return &fs->files[i];
        }
    }
    return 0;
}

FileDescriptor *FsRemove(FileSystem *fs, const char *name) {
    for (U32 i = 0; i < 64; i++) {
        if (fs->files[i].DataPointer != 0 && !StrCmp(fs->files[i].Name, name)) {
            U32 sid = fs->files[i].DataPointer / 512;
            FsFreeSectors(fs, sid, fs->files[i].DataSize/512);
            return &fs->files[i];
        }
    }
    return 0;
}