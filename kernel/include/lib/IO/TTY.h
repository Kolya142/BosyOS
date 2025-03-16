#pragma once
#include <drivers/tty.h>
#include <lib/Types.h>
#include <stdarg.h>

inline U0 TTYPrintC(U32 tty, Char c);
U0 TTYUPrintC(U32 tty, Char c);
U0 VPrintF(U32 tty, String format, va_list args);
U0 PrintF(String format, ...);
U0 SerialPrintF(String format, ...);