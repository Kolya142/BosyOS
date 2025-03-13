#include <drivers/tty.h>


U0 TTYRenderT() {
    static U32 fg = White;
    static U32 bg = Black;
    static Bool raw = False;
    for (Char c;PTermRead(VTerm, 1, &c, 1);) {
        if (TTYCursor >= TTerm.width*TTerm.height) {
            MemCpy(vga, vga+80, 4000-80);
            for (U32 i = 80 * 25 - 80; i < 80 * 25; ++i) {
                vga[i] = 0;
            }
            TTYCursor = TTerm.width * (TTerm.height - 1);
        }
        if (raw || !TTYCanonical) {
            U32 x = (TTYCursor % TTerm.width);
            U32 y = (TTYCursor / TTerm.width);
            VgaPSet(x, y, c, VgaColorGet(fg, bg));
            ++TTYCursor;
            raw = False;
            continue;
        }
        switch (c) {
            case '\r': {
                TTYCursor += TTerm.width;
                break;
            }
            case '\b': {
                TTYCursor -= 1;
                U32 x = (TTYCursor % TTerm.width);
                U32 y = (TTYCursor / TTerm.width);
                VgaPSet(x, y, ' ', VgaColorGet(fg, bg));
                break;
            }
            case '\n': {
                TTYCursor += TTerm.width - (TTYCursor % TTerm.width);
                break;
            }
            case '\t': {
                TTYCursor += 4 - (TTYCursor % 4);
                break;
            }
            case 7: {
                BeepSPC(40, 50);
                break;
            }
            case ASCIIPF1: {
                PTermRead(VTerm, 1, &c, 1);
                fg = c - ASCIIPCBlack;
                break;
            }
            case ASCIIPF2: {
                PTermRead(VTerm, 1, &c, 1);
                bg = c - ASCIIPCBlack;
                break;
            }
            case ASCIIPCtrl: {
                while (TTYCursor < TTerm.width * TTerm.height) {
                    vga[TTYCursor] = 0;
                }
                TTYCursor = 0;
            }
            case ASCIIPNextRaw: {
                raw = True;
                break;
            }
            default: {
                U32 x = (TTYCursor % TTerm.width);
                U32 y = (TTYCursor / TTerm.width);
                VgaPSet(x, y, c, VgaColorGet(fg, bg));
                ++TTYCursor;
                break;
            }
        }
    }
}