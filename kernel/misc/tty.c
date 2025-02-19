#include <libs/tty.h>
U32 TTYCursor = 0;
U0 TTYClear() {
    for (U32 i = 0; i < VGAWIDTH * VGAHEIGHT; i++) {
        vga[i] = 0x0f20;
    }
}
Bool TTYRawPrint(Char c, VgaColor fg, VgaColor bg) {
    if (c == '\n') {
        TTYCursor = TTYCursor + VGAWIDTH - TTYCursor % VGAWIDTH;
    }
    else if (c == '\r') {
        TTYCursor = TTYCursor + VGAWIDTH;
    }
    else if (c == '\t') {
        TTYCursor += 4 - TTYCursor % 4;
    }
    else if (c == '\b') {
        TTYCursor--;
        vga[TTYCursor] = 0x0000;
    }
    else if (c == (AsciiP)Left) {
        TTYCursor--;
    }
    else if (c == (AsciiP)Right) {
        TTYCursor++;
    }
    else if (c == (AsciiP)Up) {
        TTYCursor -= VGAWIDTH;
    }
    else if (c == (AsciiP)Down) {
        TTYCursor += VGAWIDTH;
    }
    else {
        vga[TTYCursor] = (U16)((VgaColorGet(fg, bg)<<8) | c);
        TTYCursor++;
    }
    if (TTYCursor < 0) {
        TTYCursor = 0;
        return False;
    }
    if (TTYCursor >= VGAWIDTH * VGAHEIGHT) {
        TTYCursor = VGAWIDTH * VGAHEIGHT - VGAWIDTH;
        return False;
    }
    VgaCursorSet(TTYCursor%VGAWIDTH, TTYCursor/VGAWIDTH);
    return True;
}
U0 TTYPrintC(Char c)
{
    stat VgaColor lfg = White;
    stat VgaColor lbg = Black;
    stat Bool lnfg = False;
    stat Bool lnbg = False;
    stat Bool lnrw = False;
    if (lnrw) {
        TTYRawPrint(c, lfg, lbg);
        lnrw = False;
    }
    else if (lnfg) {
        lfg = VgaColorFromAsciiP(c);
        lnfg = False;
    }
    else if (lnbg) {
        lbg = VgaColorFromAsciiP(c);
        lnbg = False;
    }
    else if (!(c & 0x80)) {
        TTYRawPrint(c, lfg, lbg);
    }
    else if (c == (AsciiP)Up) TTYCursor -= 80;
    else if (c == (AsciiP)Down) TTYCursor += 80;
    else if (c == (AsciiP)Left) TTYCursor --;
    else if (c == (AsciiP)Right) TTYCursor ++;
    else if (c == (AsciiP)Home) TTYCursor = 0;
    else if (c == (AsciiP)NextRaw) lnrw = True;
    else if (c == (AsciiP)F1) lnfg = True;
    else if (c == (AsciiP)F2) lnbg = True;
}
U0 TTYUPrintC(Char c) {
    stat Bool lbs = False;
    stat Bool lds = False; // $
    stat U8   ldv = 0    ;
    stat U8   lbd = 0    ;
    stat U8   lbp = 0    ;
    if (lbs) {
        if (c == '\\') 
            TTYPrintC('\\');
        if (c == 'n')
            TTYPrintC('\n');
        if (c == '$') 
            TTYPrintC('$'); 
        lbs = False;
    }
    else if (lds) {
        if (c == '!') {
            ldv = 1;
        }
        else if (c == '*') {
            ldv = 2;
        }
        else if (c == 'U') {
            TTYPrintC((AsciiP)Up);
            lds = 0;
        }
        else if (c == 'D') {
            TTYPrintC((AsciiP)Down);
            lds = 0;
        }
        else if (c == 'L') {
            TTYPrintC((AsciiP)Left);
            lds = 0;
        }
        else if (c == 'R') {
            TTYPrintC((AsciiP)Right);
            lds = 0;
        }
        else if (c == 'H') {
            TTYPrintC((AsciiP)Home);
            lds = 0;
        }
        else if (c == 'x') {
            lbp = 1;
        }
        else if (lbp) {
            lbd <<= 4;
            U8 d = 255;
            if (c >= '0' && c <= '9') {
                d = c - '0';
            }
            else if (c >= 'A' && c <= 'F') {
                d = c - 'A' + 10;
            }
            else if (c >= 'a' && c <= 'f') {
                d = c - 'a' + 10;
            }
            lbd |= d;
            lbp ++;
            if (lbp == 3) {
                TTYPrintC((AsciiP)NextRaw);
                TTYPrintC(lbd);
                lbd = 0;
                lds = False;
                lbp = 0;
            }
        }
        else {
            U8 d = 255;
            if (c >= '0' && c <= '9') {
                d = c - '0';
            }
            else if (c >= 'A' && c <= 'F') {
                d = c - 'A' + 10;
            }
            else if (c >= 'a' && c <= 'f') {
                d = c - 'a' + 10;
            }
            if (d - 255) {
                switch (ldv) {
                    case 1:
                        TTYPrintC((AsciiP)F1);
                        TTYPrintC(d+(AsciiP)CBlack);
                    break;
                    case 2:
                        TTYPrintC((AsciiP)F2);
                        TTYPrintC(d+(AsciiP)CBlack);
                    break;
                }
            }
            ldv = 0;
            lds = False;
        }
    }
    else if (c & 0x80) {
        TTYPrintC((AsciiP)NextRaw);
        TTYPrintC(c);
    }
    else if (c == '\\') {lbs = True;}
    else {
        if (c == '$') {
            lds = True;
        } 
        else {
            TTYPrintC(c);
        }
    }
}
U0 TTYPrint(String s) {
    for (U32 i = 0; s[i]; i++) {
        TTYPrintC(s[i]);
    }
}
U0 TTYUPrint(String s) {
    for (U32 i = 0; s[i]; i++) {
        TTYUPrintC(s[i]);
    }
}
U0 TTYUPrintHex(U32 i) {
    U32i t = (U32i)i;

    TTYUPrintC("0123456789ABCDEF"[t.u8[3] >> 4]);
    TTYUPrintC("0123456789ABCDEF"[t.u8[3] & 15]);
    
    TTYUPrintC("0123456789ABCDEF"[t.u8[2] >> 4]);
    TTYUPrintC("0123456789ABCDEF"[t.u8[2] & 15]);
    
    TTYUPrintC("0123456789ABCDEF"[t.u8[1] >> 4]);
    TTYUPrintC("0123456789ABCDEF"[t.u8[1] & 15]);

    TTYUPrintC("0123456789ABCDEF"[t.u8[0] >> 4]);
    TTYUPrintC("0123456789ABCDEF"[t.u8[0] & 15]);
}