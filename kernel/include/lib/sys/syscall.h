#pragma once
#include <lib/Types.h>

extern Ptr SysCallT[256];
U0 SysCallSet(Ptr func, U8 addr);
U0 SysCallInit();
U0 SysCallSetup();