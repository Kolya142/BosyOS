#pragma once
#include <lib/Types.h>

typedef struct EIFINode {
    U16 mode;
    U16 uid;
    U16 blocks[14];
} EIFINode;