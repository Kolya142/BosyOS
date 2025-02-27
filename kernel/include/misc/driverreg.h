#pragma once
#include <lib/Types.h>

U0 DriverReg(U32 d1, U32 d2, Ptr func);
U0 DriverCall(U32 d1, U32 d2, U32 id, U32 *value);