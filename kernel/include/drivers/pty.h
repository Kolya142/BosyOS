#pragma once
#include <lib/memory/Collects.h>
#include <lib/Types.h>
// Note: pd is PipeDescriptor

typedef struct PTY {
    U32 cursor;

    U32 width;
    U32 height;

    FIFO in;
    FIFO out;
    FIFO err;
} PTY;


U32 PTYWrite(PTY *pty, U32 pd, String content, U32 count);
U32 PTYRead(PTY *pty, U32 pd, String content, U32 count);
U32 PTYMake(PTY *pty, U32 size, U32 width, U32 height);