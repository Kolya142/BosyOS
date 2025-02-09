#include "fs.h"

void FInit(FileSystem *fs) {
    FsInit(fs);
}

File *FOpen(FileSystem *fs, const char *filename) {
    File *fptr = HeapAlloc(sizeof(File));
    for (int i = 0; i < 64; i++) {
        if (fs->files[i].DataPointer != 0 && !StrCmp(fs->files[i].Name, filename)) {
            fptr->ptr = fs->files[i].DataPointer;
            break;
        }
    }
    fptr->i = 0;
    return fptr;
}

File *FCreate(FileSystem *fs, const char *filename) {
    FsCreate(fs, filename);
    return FOpen(fs, filename);
}

int FRead(FileSystem *fs, File *fptr, char *buf, U32 size) {
    MemCpy(buf, &fs->data[fptr->ptr]+fptr->i, size);
    fptr->i += size;
    if (fptr->i >= fs->files[fptr->ptr].DataSize) {
        return 0;
    }
    return 1;
}

int FWrite(FileSystem *fs, File *fptr, const char *buf, U32 size) {
    MemCpy(&fs->data[fptr->ptr]+fptr->i, buf, size);
    fptr->i += size;
    if (fptr->i >= fs->files[fptr->ptr].DataSize) {
        return 0;
    }
    return 1;
}
void FClose(FileSystem *fs, File *fptr) {
    HeapFreePtr(fptr);
}