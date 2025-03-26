#include <drivers/video/vga.h>
#include <drivers/sys/beep.h>
#include <lib/graphics/Graphics.h>
#include <drivers/tty.h>
extern unsigned char TTYFont[256][8];

U0 TTYRenderGS() {
    // U32 cc = 0; // FIXME
    // U32 y = TTYGSY;
    // for (Char c;PTermRead(VTerm, 1, &c, 1);) {
    //     U32 x = cc * 8 + TTYGSX;
    //     VRMDrawRect(vec2(x, y), vec2(x+8, y+8), 0);
    //     for (U32 i = 0; i < 5; ++i) {
    //         for (U32 j = 0; j < 5; ++j) {
    //             Bool bit = (TTYFont[c][i] >> (4-j)) & 1;
    //             VRMPSet(x+j, y+i, bit ? 15 : 0);
    //         }
    //     }
    //     ++cc;
    // }
}

static U0 TPutC(Char chr, U32 cur, U8 bg, U8 fg) {
    U32 x = (cur % (WIDTH/8)) * 8;
    U32 y = (cur / (WIDTH/8)) * 8;
    VRMDrawRect(vec2(x, y), vec2(x+8, y+8), bg);
    for (U32 i = 0; i < 8; ++i) {
        for (U32 j = 0; j < 8; ++j) {
            Bool bit = (((U16)TTYFont[chr][i]) >> (7-j)) & 1;
            VRMPSet(x+j, y+i, bit ? fg : bg);
        }
    }
}

U0 TTYRenderG(TTY *this, U32 ttyid) {
    if (ttyid != TTYCurrent) {
        return;
    }
    U32 *fg = &((U32*)this->data)[0];
    U32 *bg = &((U32*)this->data)[1];
    if (*fg >= 16) {
        *fg = White;
    }
    if (*bg >= 16) {
        *bg = Black;
    }
    Bool raw = False;
    for (Char c;PTYRead(this->pty, 1, &c, 1);) {
        if (this->pty->cursor >= (WIDTH/8)*(HEIGHT/8)) {
            MemCpy(VRM, VRM+WIDTH*8, (WIDTH*HEIGHT)-WIDTH*8);
            VRMDrawRect(vec2(0, HEIGHT-8), vec2(WIDTH, HEIGHT), *bg);
            this->pty->cursor = (WIDTH/8) * (HEIGHT/8-1);
        }
        if (raw) {
            TPutC(c, this->pty->cursor, *bg, *fg);
            ++this->pty->cursor;
            raw = False;
            continue;
        }
        switch (c) {
            case '\r': {
                this->pty->cursor -= (this->pty->cursor % (WIDTH/8));
                break;
            }
            case '\b': {
                this->pty->cursor -= 1;
                U32 x = (this->pty->cursor % (WIDTH/8)) * 8;
                U32 y = (this->pty->cursor / (WIDTH/8)) * 8;
                VRMDrawRect(vec2(x, y), vec2(x+8, y+8), *bg);
                break;
            }
            case '\n': {
                this->pty->cursor += (WIDTH/8) - (this->pty->cursor % (WIDTH/8));
                break;
            }
            case '\t': {
                this->pty->cursor += 4 - (this->pty->cursor % 4);
                break;
            }
            case 7: {
                BeepSPC(40, 50);
                break;
            }
            case ASCIIPF1: {
                if (PTYRead(this->pty, 1, &c, 1)) {
                    *fg = c - ASCIIPCBlack;
                }
                break;
            }
            case ASCIIPF2: {
                if (PTYRead(this->pty, 1, &c, 1)) {
                    *bg = c - ASCIIPCBlack;
                }
                break;
            }
            case ASCIIPCtrl: {
                VRMClear(*bg);
                this->pty->cursor = 0;
                break;
            }
            case ASCIIPNextRaw: {
                raw = True;
                break;
            }
            case ASCIIPHome: {
                this->pty->cursor = 0;
                break;
            }
            default: {
                TPutC(c, this->pty->cursor, *bg, *fg);
                ++this->pty->cursor;
                break;
            }
        }
    }
}