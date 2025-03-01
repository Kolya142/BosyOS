#include <misc/driverreg.h>
#include <drivers/vga.h>
#include <lib/ASCIIP.h>
#include <lib/MemLib.h>
#include <lib/Time.h>

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

#define MISC_OUTPUT    0x3C2
#define SEQ_INDEX      0x3C4
#define SEQ_DATA       0x3C5
#define CRTC_INDEX     0x3D4
#define CRTC_DATA      0x3D5
#define GRAPHICS_INDEX 0x3CE
#define GRAPHICS_DATA  0x3CF
#define ATTRIBUTE_INDEX 0x3C0
#define ATTRIBUTE_WRITE 0x3C0
#define INPUT_STATUS   0x3DA

U0 VgaGraphicsSet() {
    POut(MISC_OUTPUT, 0x63);

    POut(SEQ_INDEX, 0x00); POut(SEQ_DATA, 0x03);
    POut(SEQ_INDEX, 0x01); POut(SEQ_DATA, 0x01);
    POut(SEQ_INDEX, 0x02); POut(SEQ_DATA, 0x0F);
    POut(SEQ_INDEX, 0x03); POut(SEQ_DATA, 0x00);
    POut(SEQ_INDEX, 0x04); POut(SEQ_DATA, 0x0E);

    POut(CRTC_INDEX, 0x11);
    POut(CRTC_DATA, PIn(CRTC_DATA) & 0x7F);
    U8 crtc[] = {
    //  0     1     2     3     4     5     6     7
        0x5F, 0x4F, 0x55, 0x82, 0x54, 0x80, 0xBF, 0x1F,  
    //  8     9     A     B     C     D     E     F
        0x00, 0x01, 0x0D, 0x0E, 0x00, 0x00, 0x00, 0x00,  
    //  10    11    12    13    14    15    16    17    18
        0x9C, 0x0E, 0x8F, 0x28, 0x40, 0x96, 0xB9, 0xA3, 0xFF  
    };
    for (int i = 0; i < 25; i++) {
        POut(CRTC_INDEX, i);
        POut(CRTC_DATA, crtc[i]);
    }

	SleepM(30);
	
	POut(GRAPHICS_INDEX, 0x00); POut(GRAPHICS_DATA, 0x00);
    POut(GRAPHICS_INDEX, 0x01); POut(GRAPHICS_DATA, 0x00);
    POut(GRAPHICS_INDEX, 0x02); POut(GRAPHICS_DATA, 0x00);
    POut(GRAPHICS_INDEX, 0x03); POut(GRAPHICS_DATA, 0x00);
    POut(GRAPHICS_INDEX, 0x04); POut(GRAPHICS_DATA, 0x00);
    POut(GRAPHICS_INDEX, 0x05); POut(GRAPHICS_DATA, 0x40);
    POut(GRAPHICS_INDEX, 0x06); POut(GRAPHICS_DATA, 0x05);
    POut(GRAPHICS_INDEX, 0x07); POut(GRAPHICS_DATA, 0x0F);
	
	SleepM(30);
	
	POut(SEQ_INDEX, 0x02); POut(SEQ_DATA, 0x0F);
	POut(SEQ_INDEX, 0x04); POut(SEQ_DATA, 0x0E);
	

    for (U8 i = 0; i < 21; i++) {
        (void)PIn(INPUT_STATUS); // Reset flip-flop
        POut(ATTRIBUTE_INDEX, i);
        POut(ATTRIBUTE_WRITE, (i < 16) ? i : 0x0F);
    }

    (void)PIn(INPUT_STATUS);
    POut(ATTRIBUTE_INDEX, 0x20);

    // POut(CRTC_INDEX, 0x09); Setuped in crtc table
    for (U32 j = 0; 0; ++j) {
        for (U32 i = 0; i < 64000; ++i) {
            ((U8*)0xA0000)[i] = (i / 320 + j) % 16;
        }
    }
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
static U0 VgaDriverHandler(U32 id, U32 *value) {
    switch (id) {
        case 0:
            VgaGraphicsSet();
            break;
        default:
            *value = 0;
            break;
    }
}
U0 VgaInit() {
    DriverReg(0xebbfd78c, 0x59687d45, VgaDriverHandler);
}