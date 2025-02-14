#pragma once
#include <libs/types.h>

extern Ptr SysCallT[256];
U0 SysCallSet(Ptr func, U8 addr);
U0 SysCallInit();