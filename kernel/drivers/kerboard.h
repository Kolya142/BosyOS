#include <arch/ports.h>
#include <lib/asciip.h>
#include <lib/string.h>
#include <drivers/screen.h>
#include <lib/screenlib.h>
#include <drivers/driverreg.h>
#pragma once
extern char KeyboardShift;
extern char KeyboardCtrl;
extern U8 KeyboardKey;
extern U8 KeyboardCode;
// void KeyboardInit();
void KeyboardAsk();
U8 KeyboardScanCodeToKey(U8 code);
void WriteKeyboardKey(U8 key, U8 c);