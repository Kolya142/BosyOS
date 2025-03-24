#include <misc/driverreg.h>
#include <drivers/video/vga.h>
#include <lib/ASCIIP.h>
#include <lib/memory/MemLib.h>
#include <lib/time/Time.h>

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
    asmv("cli");
    POut(MISC_OUTPUT, 0x63);

    POut(SEQ_INDEX, 0x00); POut(SEQ_DATA, 0x03);
    POut(SEQ_INDEX, 0x01); POut(SEQ_DATA, 0x01);
    POut(SEQ_INDEX, 0x02); POut(SEQ_DATA, 0x0F);
    POut(SEQ_INDEX, 0x03); POut(SEQ_DATA, 0x00);
    POut(SEQ_INDEX, 0x04); POut(SEQ_DATA, 0x0E);

    POut(CRTC_INDEX, 0x11);
    POut(CRTC_DATA, PIn(CRTC_DATA) & 0x7F);
    U8 crtc[] = {
        0x5f, 0x4f, 0x50, 0x82, 0x54, 0x80, 0xbf, 0x1f,
        0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x9c, 0x8e, 0x8f, 0x28, 0x40, 0x96, 0xb9, 0xa3,
        0xff
    };
    for (int i = 0; i < 25; i++) {
        POut(CRTC_INDEX, i);
        POut(CRTC_DATA, crtc[i]);
    }
	
	POut(GRAPHICS_INDEX, 0x00); POut(GRAPHICS_DATA, 0x00);
    POut(GRAPHICS_INDEX, 0x01); POut(GRAPHICS_DATA, 0x00);
    POut(GRAPHICS_INDEX, 0x02); POut(GRAPHICS_DATA, 0x00);
    POut(GRAPHICS_INDEX, 0x03); POut(GRAPHICS_DATA, 0x00);
    POut(GRAPHICS_INDEX, 0x04); POut(GRAPHICS_DATA, 0x00);
    POut(GRAPHICS_INDEX, 0x05); POut(GRAPHICS_DATA, 0x40);
    POut(GRAPHICS_INDEX, 0x06); POut(GRAPHICS_DATA, 0x05);
    POut(GRAPHICS_INDEX, 0x07); POut(GRAPHICS_DATA, 0x0F);
	
	POut(SEQ_INDEX, 0x02); POut(SEQ_DATA, 0x0F);
	POut(SEQ_INDEX, 0x04); POut(SEQ_DATA, 0x0E);
	
    U8 attr[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x41, 0x00, 0x0f, 0x00,
    };

    for (U8 i = 0; i < 21; i++) {
        (void)PIn(INPUT_STATUS); // Reset flip-flop
        POut(ATTRIBUTE_INDEX, i);
        POut(ATTRIBUTE_WRITE, attr[i]);
    }

    (void)PIn(INPUT_STATUS);
    POut(ATTRIBUTE_INDEX, 0x20);

    asmv("sti");

    // POut(CRTC_INDEX, 0x09); Setuped in crtc table
    // for (U32 j = 0; 0; ++j) {
    //     for (U32 i = 0; i < 64000; ++i) {
    //         ((U8*)0xA0000)[i] = (i / 320 + j) % 16;
    //     }
    // }
}