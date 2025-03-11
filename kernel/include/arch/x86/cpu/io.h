#pragma once
#include <lib/Types.h>

U0 POut(U16 port, U8 data);
U8 PIn(U16 port);
U0 POut16(U16 port, U16 data);
U16 PIn16(U16 port);
U0 POut32(U32 port, U16 data);
U32 PIn32(U16 port);