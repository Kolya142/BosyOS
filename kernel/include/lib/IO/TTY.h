#pragma once
#include <drivers/serial/serial.h>
#include <drivers/input/keyboard.h>
#include <misc/wordgen.h>
#include <lib/graphics/Graphics.h>
#include <lib/memory/MemLib.h>
#include <lib/strings/String.h>
#include <drivers/sys/beep.h>
#include <lib/Types.h>
#include <drivers/video/vga.h>
#include <drivers/tty.h>
#include <lib/ASCIIP.h>
#include <lib/math/FDL.h>
#include <lib/IO/PTY.h>
#include <stdarg.h>

U0 TTYInit();
U0 TTYClear();
U0 TTYRawPrint(Char c, VgaColor fg, VgaColor bg);
U0 TTYPrintC(Char c);
U0 TTYUPrintC(Char c);
U0 SerialPrintF(String format, ...);
U0 PrintF(String format, ...);

U0 TTYUPrint(String s);
U0 TTYUPrintHex(U32 i);
U0 TTYUPrintDec(U32 a);
U0 TTYUPrintDecI(I32 a);
U0 VPrintF(String format, va_list args);