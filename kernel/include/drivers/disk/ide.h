#pragma once
#include <lib/Types.h>
#include <arch/x86/cpu/io.h>

typedef struct IDEAsk {
    Ptr buf;
    U32 start;
    U8 end;
} IDEAsk;

U0 ATARead(Bool slave, Ptr buf, U32 start, U8 count);
U0 ATAWrite(Bool slave, Ptr buf, U32 start, U8 count);

U0 IDEInit();