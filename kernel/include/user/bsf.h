#pragma once
#include <libs/types.h>
typedef struct BsfHeader {
    U32 Magic;
    U32 CodeS;
} BsfHeader;
typedef struct BsfApp {
    BsfHeader header;
    Ptr      data;
} BsfApp;

Bool BsfExec(BsfApp *app);