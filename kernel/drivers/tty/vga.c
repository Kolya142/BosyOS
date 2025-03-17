#include <drivers/video/vga.h>
#include <drivers/sys/beep.h>
#include <drivers/tty.h>

static U0 TPutC(Char chr, U32 cur, U8 col) {
    vga[cur] = (col << 8) | chr;
}

U0 TTYRenderT(TTY *this, U32 ttyid) {
    U32 *fg = &((U32*)this->data)[0];
    U32 *bg = &((U32*)this->data)[1];
    if (*fg >= 16) {
        *fg = White;
    }
    if (*bg >= 16) {
        *bg = Black;
    }
    Bool raw = False;
    Char c;
    while (PTYRead(this->pty, 1, &c, 1)) {
        if (this->pty->cursor >= 80*25) {
            MemCpy(vga, vga+80, 4000-80);
            for (U32 i = 80 * 25 - 80; i < 80 * 25; ++i) {
                vga[i] = 0;
            }
            this->pty->cursor = 80 * 24;
        }
        if (raw) {
            TPutC(c, this->pty->cursor, VgaColorGet(*fg, *bg));
            ++this->pty->cursor;
        }
        switch (c) {
            case '\r': {
                this->pty->cursor += 80;
                break;
            }
            case '\b': {
                this->pty->cursor -= 1;
                U32 x = (this->pty->cursor % 80);
                U32 y = (this->pty->cursor / 80);
                VgaPSet(x, y, ' ', VgaColorGet(*fg, *bg));
                break;
            }
            case '\n': {
                this->pty->cursor += 80 - (this->pty->cursor % 80);
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
                PTYRead(this->pty, 1, &c, 1);
                *fg = c - ASCIIPCBlack;
                break;
            }
            case ASCIIPF2: {
                PTYRead(this->pty, 1, &c, 1);
                *bg = c - ASCIIPCBlack;
                break;
            }
            case ASCIIPCtrl: {
                for (U32 i = 0; i < 80*25; ++i) {
                    vga[i] = 0;
                }
                this->pty->cursor = 0;
            }
            case ASCIIPNextRaw: {
                raw = True;
                break;
            }
            default: {
                TPutC(c, this->pty->cursor, VgaColorGet(*fg, *bg));
                ++this->pty->cursor;
                break;
            }
        }
    }
}