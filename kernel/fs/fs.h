#include <fs/sykofs.h>
#include <kernel/alloc.h>
#pragma once

typedef struct File {
    U32 ptr;
    U32 i;
} File;

void FInit(FileSystem *fs);
File *FOpen(FileSystem *fs, const char *filename);
File *FCreate(FileSystem *fs, const char *filename);
int FRead(FileSystem *fs, File *fptr, char *buf, U32 size);
int FWrite(FileSystem *fs, File *fptr, const char *buf, U32 size);
void FClose(FileSystem *fs, File *fptr);