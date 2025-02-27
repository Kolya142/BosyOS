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
} BsfMeta;

typedef struct BsfApp {
    BsfHeader header;
    Ptr      data;
} __attribute__((packed)) BsfApp;

BsfApp BsfFromBytes(Byte *app);

Bool BsfExec(BsfApp *app);