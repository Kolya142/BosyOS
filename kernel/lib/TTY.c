#include <drivers/serial.h>
#include <misc/wordgen.h>
#include <lib/Graphics.h>
#include <lib/MemLib.h>
#include <lib/TTY.h>
#include <lib/FDL.h>
#include <stdarg.h>
U32 TTYCursor = 0;
VgaColor TTYlfg = White;
VgaColor TTYlbg = Black;
U0(*TTYPuter)(Char) = TTYPutT;
U32 TTYWidth  = 80;
U32 TTYHeight = 25;

U0 TTYClear() {
    TTYCursor = 0;
    for (U32 i = 0; i < TTYWidth * TTYHeight; i++) {
        TTYPuter(' ');
        ++TTYCursor;
    }
}
Bool TTYRawPrint(Char c, VgaColor fg, VgaColor bg) {
    if (c == '\n') {
        TTYCursor = TTYCursor + TTYWidth - TTYCursor % TTYWidth;
    }
    else if (c == '\r') {
        TTYCursor = TTYCursor + TTYWidth;
    }
    else if (c == '\t') {
        TTYCursor += 4 - TTYCursor % 4;
    }
    else if (c == '\b') {
        TTYCursor--;
        TTYPuter(' ');
    }
    else if (c == (AsciiP)ASCIIPLeft) {
        TTYCursor--;
    }
    else if (c == (AsciiP)ASCIIPRight) {
        TTYCursor++;
    }
    else if (c == (AsciiP)ASCIIPUp) {
        TTYCursor -= TTYWidth;
    }
    else if (c == (AsciiP)ASCIIPDown) {
        TTYCursor += TTYWidth;
    }
    else {
        U8 bg1 = TTYlbg;
        U8 fg1 = TTYlfg;
        TTYlbg = bg;
        TTYlfg = fg;
        TTYPuter(c);
        TTYlbg = bg1;
        TTYlfg = fg1;
        TTYCursor++;
    }
    if (TTYCursor >= TTYWidth * TTYHeight) {
        MemCpy(vga - 80, vga, 4000); // TODO: add more crosstty
        for (U32 i = TTYWidth * TTYHeight - TTYWidth; i < TTYWidth * TTYHeight; ++i) {
            TTYCursor = i;
            TTYPuter(' ');
        }
        TTYCursor = TTYWidth * TTYHeight - TTYWidth;
        return False;
    }
    VgaCursorSet(TTYCursor%TTYWidth, TTYCursor/TTYWidth);
    return True;
}
U0 TTYPrintC(Char c)
{
    stat Bool lnfg = False;
    stat Bool lnbg = False;
    stat Bool lnrw = False;
    if (lnrw) {
        TTYRawPrint(c, TTYlfg, TTYlbg);
        lnrw = False;
    }
    else if (lnfg) {
        TTYlfg = VgaColorFromAsciiP(c);
        lnfg = False;
    }
    else if (lnbg) {
        TTYlbg = VgaColorFromAsciiP(c);
        lnbg = False;
    }
    else if (!(c & 0x80)) {
        TTYRawPrint(c, TTYlfg, TTYlbg);
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
                case 'b': {
                    U16 s = va_arg(args, U32);
                    for (I8 i = 15; i >= 0; --i) {
                        TTYUPrintC((s & (1 << i)) ? '1' : '0');
                    }
                } break;                
                case 'd': {
                    U32 s = va_arg(args, U32);
                    TTYUPrintDec(s);
                } break;
                case 'i': {
                    I32 s = va_arg(args, I32);
                    TTYUPrintDec(s);
                } break;
                case 'p': {
                    U32 s = va_arg(args, U32);
                    TTYUPrint("0x");
                    TTYUPrintHex(s);
                } break;
                case 'x': {
                    U32 s = va_arg(args, U32);
                    TTYUPrintHex(s);
                } break;
                case 'X': {
                    U32 s = va_arg(args, U32);
                    U16i t = (U16i)(U16)s;
                    
                    TTYUPrintC("0123456789ABCDEF"[t.u8[1] >> 4]);
                    TTYUPrintC("0123456789ABCDEF"[t.u8[1] & 15]);
                
                    TTYUPrintC("0123456789ABCDEF"[t.u8[0] >> 4]);
                    TTYUPrintC("0123456789ABCDEF"[t.u8[0] & 15]);
                } break;
                case 'C': {
                    U32 s = va_arg(args, U32);                
                    TTYUPrintC("0123456789ABCDEF"[s >> 4]);
                    TTYUPrintC("0123456789ABCDEF"[s & 15]);
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
U0 TTYPutG(Char c) {
    U32 cur = TTYCursor;
    U32 x = (cur % TTYWidth) * 6;
    U32 y = (cur / TTYWidth) * 6 + 1;
    VRMDrawRect(vec2(x, y), vec2(x+6, y+6), TTYlbg);
    VRMDrawSprite(vec2(x, y), vec2(5, 5), TTYlfg, TTYlbg, TTYFont[c]);
}
U0 TTYPutT(Char c) {
    vga[TTYCursor] = ((((U16)TTYlbg << 4) | (U16)TTYlfg) << 8) | (U16)c;
}
U0 TTYPutS(Char c) {
    U32 x = TTYCursor % 80 + 1;
    U32 y = TTYCursor / 80 + 1;
    static const U8 fgs[] = {
        30, 34, 32, 36, 31, 35, 33, 90,
        37, 94, 92, 96, 91, 95, 93, 97
    };
    static const U8 bgs[] = {
        40, 44, 42, 46, 41, 45, 43, 100,
        47, 104, 102, 106, 101, 105, 103, 107
    };
    U8 bg = bgs[TTYlbg&0xf];
    U8 fg = fgs[TTYlfg&0xf];
    U8 buf[0x1B];
    U32 index = 0;
    buf[index++] = ('\x1b');
    buf[index++] = ('[');
    buf[index++] = ('0' + (y / 100) % 10);
    buf[index++] = ('0' + (y / 10) % 10);
    buf[index++] = ('0' + y % 10);
    buf[index++] = (';');
    buf[index++] = ('0' + (x / 100) % 10);
    buf[index++] = ('0' + (x / 10) % 10);
    buf[index++] = ('0' + x % 10);
    buf[index++] = ('f');

    buf[index++] = ('\x1b');
    buf[index++] = ('[');
    buf[index++] = ('0' + (bg / 100) % 10);
    buf[index++] = ('0' + (bg / 10) % 10);
    buf[index++] = ('0' + bg % 10);
    buf[index++] = ('m');
    
    buf[index++] = ('\x1b');
    buf[index++] = ('[');
    buf[index++] = ('0' + (fg / 100) % 10);
    buf[index++] = ('0' + (fg / 10) % 10);
    buf[index++] = ('0' + fg % 10);
    buf[index++] = ('m');

    buf[index++] = c;

    buf[index++] = ('\x1b');
    buf[index++] = ('[');
    buf[index++] = ('0');
    buf[index++] = ('m');
    
    SerialWrite(c);
    // for (U32 i = 0; i < 0x1B; ++i) {
    //     SerialWrite(buf[i]);
    // }
}
U0 TTYSwitch(TTYContext tc) {
    static TTYContext prev = TTYC_VGA;
    static TTYContext curr = TTYC_VGA;
    switch (tc) {
        case TTYC_VGA:
            TTYPuter = TTYPutT;
            TTYWidth = 80;
            TTYHeight = 25;
            prev = curr;
            curr = tc;
            break;
        case TTYC_320:
            TTYPuter = TTYPutG;
            TTYWidth = 320/5;
            TTYHeight = 200/5;
            prev = curr;
            curr = tc;
            break;
        case TTYC_RES:
            TTYSwitch(prev);
            break;
        case TTYC_SER:
            TTYPuter = TTYPutS;
            TTYWidth = 80;
            TTYHeight = 25;
            prev = curr;
            curr = tc;
            break;
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
U0 TTYUPrintDec(U32 a) {
    if (!a) {
        TTYUPrintC('0');
        return;
    }
    U32 S = FDL(a);
    String s = MAlloc(S + 1);
    MemSet(s, 0, S + 1);
    U32 p = 0;
    while (a) {
        s[S-1-p] = '0' + (a % 10);
        a /= 10;
        p++;
    }
    TTYUPrint(s);
    MFree(s);
}
U0 TTYUPrintDecI(I32 a) {
    if (a < 0) {
        TTYUPrintC('-');
        a = -a;
    }
    TTYUPrintDec(a);
}