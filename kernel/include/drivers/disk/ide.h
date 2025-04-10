#pragma once
#include <drivers/disk/atapi.h>
#include <arch/x86/cpu/io.h>
#include <lib/Types.h>

Bool ATARead(Bool slave, Ptr buf, U32 start, U8 count);
Bool ATAWrite(Bool slave, Ptr buf, U32 start, U8 count);

U0 IDEInit();