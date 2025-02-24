#pragma once
#include <lib/String.h>
#include <lib/MemLib.h>
#include <lib/Types.h>
#include <lib/DAT.h>
// BOsy Tiny FileSystem

/*
IDs:
0x10 - BOTFSHeader
0x11... - BOTFS Data
*/

typedef struct BOTFSHeader {
    U32 magic; // 0x674E1D8F
    U8 version;
    U16 blocks_count;
    U32 files_start;  // in Bytes
    U32 bitmap_start; // in Bytes
    U16 data_start;   // in Segments
    U8 flags;
} __attribute__((packed)) BOTFSHeader;

typedef struct BOTFSFile {
    Char name[12];
    U8 flags;
    U16 start;
    U16 shift;
    U16 size;
    Bool exists;
    U8 zero[2];
} __attribute__((packed)) BOTFSFile;

extern BOTFSHeader BFSGlob;

U0 BOTFSInit();
U0 BOTFSFormat();
U0 BOTFSFlush();
U0 BOTFSDRead(U16 seg, U16 start, U16 end, Ptr buf);
U0 BOTFSDWrite(U16 seg, U16 start, U16 end, Ptr buf);

U32 BOTFSFind(String name);

U0 BOTFSRead(String name, Ptr buf, U16 shift, U16 count);
U0 BOTFSWrite(String name, Ptr buf, U16 shift, U16 count);
U0 BOTFSDelete(String name);
U0 BOTFSCreate(String name, U16 size);
U0 BOTFSList(String *buf, U16 max_files);
BOTFSFile *BOTFSFGet();
U8 *BOTFSBGet();
U0 BOTFSFPut(BOTFSFile *f);
U0 BOTFSBPut(U8 *f);