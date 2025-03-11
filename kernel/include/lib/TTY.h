#pragma once
#include <drivers/serial.h>
#include <misc/wordgen.h>
#include <lib/Graphics.h>
#include <lib/MemLib.h>
#include <lib/String.h>
#include <arch/beep.h>
#include <lib/Types.h>
#include <drivers/vga.h>
#include <lib/ASCIIP.h>
#include <lib/FDL.h>
#include <lib/PTY.h>
#include <stdarg.h>

typedef enum {
    TTYC_VGA,
    TTYC_320,
    TTYC_SER,
    TTYC_RES
} TTYContext;

typedef struct TTY
{
    U0(*render)();
    U32 width;
    U32 height;
} TTY;

extern U32 TTYCursor;
extern PTerm VTerm;
extern TTY TTerm;
extern unsigned char TTYFont[256][5];
extern U32 TTYGSX;
extern U32 TTYGSY;

U0 TTYInit();
U0 TTYClear();
U0 TTYRawPrint(Char c, VgaColor fg, VgaColor bg);
U0 TTYPrintC(Char c);
U0 TTYUPrintC(Char c);
U0 SerialPrintF(String format, ...);
U0 PrintF(String format, ...);
U0 TTYRenderGS();
U0 TTYRenderG();
U0 TTYRenderT();
U0 TTYRenderS();
U0 TTYUPrint(String s);
U0 TTYUPrintHex(U32 i);
U0 TTYUPrintDec(U32 a);
U0 TTYUPrintDecI(I32 a);