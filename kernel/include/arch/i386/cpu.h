#pragma once
#include <lib/Types.h>

U0 POut(U16 port, U8 data);
U8 PIn(U16 port);
U0 POut16(U16 port, U16 data);
U16 PIn16(U16 port);
U0 POut32(U16 port, U32 data);
U32 PIn32(U16 port);

U0 CpuNameGet(String name);
U32 MemorySize();
U0 PICMap();
U0 PICClearMask(U8 irq);

U0 PICMap();
U0 PICClearMask(U8 irq);

U0 MSRGet(U32 msr, U32 *lo, U32 *hi);
U0 MSRSet(U32 msr, U32 lo, U32 hi);