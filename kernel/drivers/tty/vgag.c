#include <drivers/tty.h>


U0 TTYRenderGS() {
    U32 cc = 0;
    U32 y = TTYGSY;
    for (Char c;PTermRead(VTerm, 1, &c, 1);) {
        U32 x = cc * 6 + TTYGSX;
        VRMDrawRect(vec2(x, y), vec2(x+6, y+6), 0);
        for (U32 i = 0; i < 5; ++i) {
            for (U32 j = 0; j < 5; ++j) {
                Bool bit = (TTYFont[c][i] >> (4-j)) & 1;
                VRMPSet(x+j, y+i, bit ? 15 : 0);
            }
        }
        ++cc;
    }
}
U0 TTYRenderG() {
    static U32 fg = White;
    static U32 bg = Black;
    static Bool raw = False;
    for (Char c;PTermRead(VTerm, 1, &c, 1);) {
        TTYBuffers[TTermID][TTYBuffersIndex[TTermID]] = c;
        TTYBuffersIndex[TTermID] = (TTYBuffersIndex[TTermID] + 1) % sizeof(TTYBuffers[TTermID]);
        if (TTYCursor >= TTerm.width*TTerm.height) {
            MemCpy(VRM, VRM+320*6, 64000-320*6);
            for (U32 i = TTerm.width * TTerm.height - TTerm.width; i < TTerm.width * TTerm.height; ++i) {
                TTYCursor = i;
                U32 x = (TTYCursor % TTerm.width) * 6;
                U32 y = (TTYCursor / TTerm.width) * 6 + 1;
                VRMDrawRect(vec2(x, y), vec2(x+6, y+6), bg);
            }
            TTYCursor = TTerm.width * (TTerm.height - 1);
        }
        if (raw || !TTYCanonical) {
            U32 x = (TTYCursor % TTerm.width) * 6;
            U32 y = (TTYCursor / TTerm.width) * 6 + 1;
            VRMDrawRect(vec2(x, y), vec2(x+6, y+6), bg);
            for (U32 i = 0; i < 5; ++i) {
                for (U32 j = 0; j < 5; ++j) {
                    Bool bit = (TTYFont[c][i] >> (4-j)) & 1;
                    VRMPSet(x+j, y+i, bit ? fg : bg);
                }
            }
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
                U32 x = (TTYCursor % TTerm.width) * 6;
                U32 y = (TTYCursor / TTerm.width) * 6 + 1;
                VRMDrawRect(vec2(x, y), vec2(x+6, y+6), bg);
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
                VRMDrawRect(vec2(0, 0), vec2(TTerm.width * 6, TTerm.height * 6), bg);
                TTYCursor = 0;
                break;
            }
            case ASCIIPNextRaw: {
                raw = True;
                break;
            }
            case ASCIIPHome: {
                TTYCursor = 0;
                break;
            }
            default: {
                U32 x = (TTYCursor % TTerm.width) * 6;
                U32 y = (TTYCursor / TTerm.width) * 6 + 1;
                VRMDrawRect(vec2(x, y), vec2(x+6, y+6), bg);
                for (U32 i = 0; i < 5; ++i) {
                    for (U32 j = 0; j < 5; ++j) {
                        Bool bit = (TTYFont[c][i] >> (4-j)) & 1;
                        VRMPSet(x+j, y+i, bit ? fg : bg);
                    }
                }
                ++TTYCursor;
                break;
            }
        }
    }
}