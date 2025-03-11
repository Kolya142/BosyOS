#pragma once
#include <lib/Types.h>
#include <arch/x86/cpu/io.h>
#define VGAADDR 0xB8000
#define VGAWIDTH 80
#define VGAHEIGHT 25
extern U16 *vga;
typedef enum VgaColor {
    Black = 0,
    DBlue,
    DGreen,
    DCyan,
    DRed,
    Purple,
    Brown,
    Gray,
    DGray,
    Blue,
    Green,
    Cyan,
    Red,
    Magenta,
    Yellow,
    White,
} VgaColor;
Bool VgaPSet(U16 x, U16 y, U8 ch, U8 c);
U8 VgaColorGet(VgaColor fg, VgaColor bg);
VgaColor VgaColorFromAsciiP(Char c);
U0 VgaCursorEnable();
U0 VgaCursorDisable();
U0 VgaCursorSetR(U32 c);
U0 VgaCursorSet(U16 x, U16 y);
U0 VgaGraphicsSet();
// It's doen't work
U0 VgaBlinkingSet(Bool state);

U0 VgaFontLoad(const U8 *font);
U0 VgaInit();