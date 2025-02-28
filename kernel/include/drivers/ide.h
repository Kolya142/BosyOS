#pragma once
#include <lib/Types.h>
#include <arch/io.h>

typedef struct IDEAsk {
    Ptr buf;
    U32 start;
    U8 end;
} IDEAsk;

U0 ATARead(Ptr buf, U32 start, U8 count);
U0 ATAWrite(Ptr buf, U32 start, U8 count);

U0 IDEInit();