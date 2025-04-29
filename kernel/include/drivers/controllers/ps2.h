#pragma once
#include <lib/Types.h>

U0 PS2Wait(U8 hw);
U0 PS2Clean();
U0 PS2Write(U8 cmd, U8 byte);
U8 PS2Read();
U0 PS2Init();