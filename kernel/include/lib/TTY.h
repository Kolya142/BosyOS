#pragma once
#include <lib/Types.h>
#include <drivers/vga.h>
#include <lib/ASCIIP.h>
extern U32 TTYCursor;
U0 TTYClear();
Bool TTYRawPrint(Char c, VgaColor fg, VgaColor bg);
U0 TTYPrintC(Char c);
U0 TTYUPrintC(Char c);
U0 TTYPrint(String s);
U0 TTYUPrint(String s);
U0 TTYUPrintHex(U32 i);
U0 TTYUPrintDec(U32 a);