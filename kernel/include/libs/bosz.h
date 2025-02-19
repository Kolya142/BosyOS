#pragma once
#include <libs/types.h>

typedef struct {
    Byte *data;
    U32 len;
} Data;

Data DeCompress(const Byte *input, U32 len);