#pragma once
#include <drivers/ide.h>
#include <lib/Types.h>

#define DAT_SECTORS 32

typedef struct DATHeader {
    U16 magic;
    U8 alloc_table[DAT_SECTORS/8];
    U16 id_table[DAT_SECTORS];
} __attribute__((packed)) DATHeader;

extern DATHeader DATHeaderGlob;

DATHeader *DATInit();

U0 DATFlush();

U16 DATFind(U16 id);

U32 DATAlloc(U16 id);
U0 DATFree(U16 id);

U0 DATWrite(U16 id, Ptr buf, U16 start, U16 count);
U0 DATRead(U16 id, Ptr buf, U16 start, U16 count);