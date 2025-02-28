#pragma once
#include <lib/Types.h>
typedef struct BsfHeader {
    U32 Magic ;
    U32 CodeS ;
    U32 BosSec;
} __attribute__((packed)) BsfHeader;
typedef struct BsfMeta
{
    U32 meta1;
    U32 meta2;
    Ptr func;
} __attribute__((packed)) BsfMeta;

typedef struct BsfApp {
    BsfHeader header;
    Ptr      data;
} __attribute__((packed)) BsfApp;
BsfMeta *BsfMetaLoad(Ptr start, U32 d1, U32 d2, U32 size);

BsfApp BsfFromBytes(Byte *app);

// 0 - error, 1 - nothing, 2+ - program return code
I32 BsfExec(BsfApp *app, U32 m1, U32 m2);