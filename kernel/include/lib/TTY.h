#pragma once
#include <lib/Types.h>
#include <drivers/vga.h>
#include <lib/ASCIIP.h>
extern U32 TTYCursor;
extern VgaColor TTYlfg;
extern VgaColor TTYlbg;
extern U8 TTYFont[256][5*5];
extern U0(*TTYPuter)(Char);
extern U32 TTYWidth;
extern U32 TTYHeight;
U0 TTYClear();
Bool TTYRawPrint(Char c, VgaColor fg, VgaColor bg);
U0 TTYPrintC(Char c);
U0 TTYUPrintC(Char c);
U0 PrintF(String format, ...);
U0 TTYPutG(Char c);
U0 TTYPutT(Char c);
U0 TTYPrint(String s);
U0 TTYUPrint(String s);
U0 TTYUPrintHex(U32 i);
U0 TTYUPrintDec(U32 a);
U0 TTYUPrintDecI(I32 a);