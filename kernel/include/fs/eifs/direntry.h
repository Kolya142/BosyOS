#pragma once
#include <lib/Types.h>

typedef struct EIFDE {
    U16 inode;
    Char name[14];
} __attribute__((packed)) EIFDE;