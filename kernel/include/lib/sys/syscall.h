#pragma once
#include <lib/Types.h>

extern Ptr SysCallT[400];
U0 SysCallSet(Ptr func, U16 addr);
U0 SysCallInit();
U0 SysCallSetup();