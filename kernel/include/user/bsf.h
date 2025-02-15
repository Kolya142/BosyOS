#pragma once
#include <libs/types.h>
typedef struct BsfHeader {
    U32 Magic;
    U32 CodeS;
} __attribute__((packed)) BsfHeader;
typedef struct BsfApp {
    BsfHeader header;
    Ptr      data;
} __attribute__((packed)) BsfApp;

Bool BsfExec(BsfApp *app);