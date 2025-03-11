#pragma once
#include <lib/memory/Collects.h>
#include <lib/Types.h>

typedef struct PTerm
{
    FIFO in;
    FIFO out;
    FIFO err;
} PTerm;

PTerm PTermInit(U32 size);
U32 PTermWrite(PTerm *term, U8 pd, Ptr buf, U32 count);
U32 PTermRead(PTerm *term, U8 pd, Ptr buf, U32 count);

