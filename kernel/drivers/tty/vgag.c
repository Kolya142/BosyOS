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

static U8 fgs[4] = {White, White, White, White};
static U8 bgs[4] = {Black, Black, Black, Black};

U0 TTYRenderG() {
    static Bool raw = False;
    for (Char c;PTermRead(VTerm, 1, &c, 1);) {
        if (TTYCursor >= TTerm.width*TTerm.height) {
            MemCpy(VRM, VRM+320*6, 64000-320*6);
            for (U32 i = TTerm.width * TTerm.height - TTerm.width; i < TTerm.width * TTerm.height; ++i) {
                TTYCursor = i;
                U32 x = (TTYCursor % TTerm.width) * 6;
                U32 y = (TTYCursor / TTerm.width) * 6 + 1;
                VRMDrawRect(vec2(x, y), vec2(x+6, y+6), bgs[TTermID]);
            }
            TTYCursor = TTerm.width * (TTerm.height - 1);
        }
        if (raw || !TTYCanonical) {
            
            U32 x = (TTYCursor % TTerm.width) * 6;
            U32 y = (TTYCursor / TTerm.width) * 6 + 1;
            VRMDrawRect(vec2(x, y), vec2(x+6, y+6), bgs[TTermID]);
            for (U32 i = 0; i < 5; ++i) {
                for (U32 j = 0; j < 5; ++j) {
                    Bool bit = (TTYFont[c][i] >> (4-j)) & 1;
                    VRMPSet(x+j, y+i, bit ? fgs[TTermID] : bgs[TTermID]);
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
                VRMDrawRect(vec2(x, y), vec2(x+6, y+6), bgs[TTermID]);
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
                if (PTermRead(VTerm, 1, &c, 1)) {
                    fgs[TTermID] = c - ASCIIPCBlack;
                }
                break;
            }
            case ASCIIPF2: {
                if (PTermRead(VTerm, 1, &c, 1)) {
                    bgs[TTermID] = c - ASCIIPCBlack;
                }
                break;
            }
            case ASCIIPCtrl: {
                VRMClear(bgs[TTermID]);
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
                VRMDrawRect(vec2(x, y), vec2(x+6, y+6), bgs[TTermID]);
                for (U32 i = 0; i < 5; ++i) {
                    for (U32 j = 0; j < 5; ++j) {
                        Bool bit = (TTYFont[c][i] >> (4-j)) & 1;
                        VRMPSet(x+j, y+i, bit ? fgs[TTermID] : bgs[TTermID]);
                    }
                }
                ++TTYCursor;
                break;
            }
        }
    }
}