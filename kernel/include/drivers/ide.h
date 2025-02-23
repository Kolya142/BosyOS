#pragma once
#include <lib/Types.h>
#include <arch/io.h>

U0 ATARead(Ptr buf, U32 start, U8 count);
U0 ATAWrite(Ptr buf, U32 start, U8 count);