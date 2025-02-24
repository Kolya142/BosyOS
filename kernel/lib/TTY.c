#include <misc/wordgen.h>
#include <lib/MemLib.h>
#include <lib/TTY.h>
#include <lib/FDL.h>
#include <stdarg.h>
U32 TTYCursor = 0;
U0 TTYClear() {
    for (U32 i = 0; i < VGAWIDTH * VGAHEIGHT; i++) {
        vga[i] = 0x0000;
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
    else if (c == (AsciiP)ASCIIPLeft) {
        TTYCursor--;
    }
    else if (c == (AsciiP)ASCIIPRight) {
        TTYCursor++;
    }
    else if (c == (AsciiP)ASCIIPUp) {
        TTYCursor -= VGAWIDTH;
    }
    else if (c == (AsciiP)ASCIIPDown) {
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
    else if (c == (AsciiP)ASCIIPUp) TTYCursor -= 80;
    else if (c == (AsciiP)ASCIIPDown) TTYCursor += 80;
    else if (c == (AsciiP)ASCIIPLeft) TTYCursor --;
    else if (c == (AsciiP)ASCIIPRight) TTYCursor ++;
    else if (c == (AsciiP)ASCIIPHome) TTYCursor = 0;
    else if (c == (AsciiP)ASCIIPNextRaw) lnrw = True;
    else if (c == (AsciiP)ASCIIPF1) lnfg = True;
    else if (c == (AsciiP)ASCIIPF2) lnbg = True;
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
            TTYPrintC((AsciiP)ASCIIPUp);
            lds = 0;
        }
        else if (c == 'D') {
            TTYPrintC((AsciiP)ASCIIPDown);
            lds = 0;
        }
        else if (c == 'L') {
            TTYPrintC((AsciiP)ASCIIPLeft);
            lds = 0;
        }
        else if (c == 'R') {
            TTYPrintC((AsciiP)ASCIIPRight);
            lds = 0;
        }
        else if (c == 'H') {
            TTYPrintC((AsciiP)ASCIIPHome);
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
                TTYPrintC((AsciiP)ASCIIPNextRaw);
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
                        TTYPrintC((AsciiP)ASCIIPF1);
                        TTYPrintC(d+(AsciiP)ASCIIPCBlack);
                    break;
                    case 2:
                        TTYPrintC((AsciiP)ASCIIPF2);
                        TTYPrintC(d+(AsciiP)ASCIIPCBlack);
                    break;
                }
            }
            ldv = 0;
            lds = False;
        }
    }
    else if (c & 0x80) {
        TTYPrintC((AsciiP)ASCIIPNextRaw);
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
U0 PrintF(String format, ...) {
    va_list args;
    va_start(args, format);
    while (*format) {
        if (*format == '%' && *(format + 1)) {
            ++format;
            switch (*format) {
                case 'c': {
                    Char c = va_arg(args, U32);
                    TTYUPrintC(c);
                } break;
                case 'w': {
                    WordGen();
                } break;
                case 's': {
                    String s = va_arg(args, String);
                    TTYUPrint(s);
                } break;
                case 'B': {
                    Bool s = va_arg(args, U32);
                    TTYUPrint(s ? "True" : "False");
                } break;
                case 'd': {
                    U32 s = va_arg(args, U32);
                    TTYUPrintDec(s);
                } break;
                case 'p':
                case 'x': {
                    U32 s = va_arg(args, U32);
                    TTYUPrintHex(s);
                } break;
                case '%': {
                    TTYUPrintC('%');
                } break;
                default:
                    TTYUPrintC('%');
                    TTYUPrintC(*format);
            }
            ++format;
            continue;
        }
        TTYUPrintC(*format);
        ++format;
    }
    va_end(args);
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
U0 TTYUPrintDec(U32 a) {
    U8 n = a < 0;
    if (n) a = -a;
    
    if (!a) {
        TTYUPrintC('0');
        return;
    }
    U32 S = FDL(a);
    String s = MAlloc(S + 1);
    MemSet(s, 0, S + 1);
    U32 p = 0;
    if (n) {
        TTYUPrintC('-');
    }
    while (a) {
        s[S-1-p] = '0' + (a % 10);
        a /= 10;
        p++;
    }
    TTYUPrint(s);
    MFree(s);
}