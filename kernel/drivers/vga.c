#include <drivers/vga.h>
#include <lib/ASCIIP.h>
#include <lib/MemLib.h>
U16 *vga = (U16*)VGAADDR;

Bool VgaPSet(U16 x, U16 y, U8 ch, U8 c)
{
    if (x < 0 || x >= VGAWIDTH || y < 0 || y >= VGAHEIGHT) return False;
    vga[y * VGAWIDTH + x] = (U16)c << 8 | ch;
    return True;
}
U8 VgaColorGet(VgaColor fg, VgaColor bg)
{
    return (bg << 4) | fg;
}
VgaColor VgaColorFromAsciiP(Char c)
{
    if (c < (AsciiP)ASCIIPCBlack) return 0;
    return c - (AsciiP)ASCIIPCBlack;
}
U0 VgaCursorEnable()
{
	POut(0x3D4, 0x0A);
	POut(0x3D5, (PIn(0x3D5) & 0xC0) | 0);

	POut(0x3D4, 0x0B);
	POut(0x3D5, (PIn(0x3D5) & 0xE0) | VGAHEIGHT);
}

U0 VgaCursorDisable()
{
	POut(0x3D4, 0x0A);
	POut(0x3D5, 0x20);
}

U0 VgaCursorSetR(U32 c)
{
	POut(0x3D4, 0x0F);
	POut(0x3D5, (U8) (c & 0xFF));
	POut(0x3D4, 0x0E);
	POut(0x3D5, (U8) ((c >> 8) & 0xFF));
}
U0 VgaCursorSet(U16 x, U16 y)
{
    U32 cur = y * VGAWIDTH + x;
	VgaCursorSetR(cur);
}

U0 VgaBlinkingSet(Bool state) {
	// POut(0x03C0, 0x10);
	// U8 m = PIn(0x03C1);
	// if (state) m |= 0x08; else m &= ~0x08;
	// POut(0x03C0, 0x10);
	// POut(0x03C0, m);
}

U0 VgaFontLoad(const U8 *font) {
	POut(0x03C4, 0x04);
	POut(0x03C5, 0x07);

	POut(0x03CE, 0x06);
	POut(0x03CF, 0x04);

	U8 *out = (U8*)0xA0000;
	MemCpy(out, font, 256*16);

	POut(0x03C4, 0x04);
	POut(0x03C5, 0x03);
}