#include <drivers/serial.h>
#include <misc/wordgen.h>
#include <lib/Graphics.h>
#include <lib/MemLib.h>
#include <lib/String.h>
#include <lib/TTY.h>
#include <lib/FDL.h>
#include <stdarg.h>
U32 TTYCursor = 0;
U32 TTYCursorX = 0;
U32 TTYCursorY = 0;
VgaColor TTYlfg = White;
VgaColor TTYlbg = Black;
U0(*TTYPuter)(Char) = TTYPutT;
U0(*TTYScroller)() = TTYScrollT;
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
        TTYScroller();
        TTYCursor = TTYWidth * TTYHeight - TTYWidth;
        return False;
    }
    VgaCursorSet(TTYCursor%TTYWidth, TTYCursor/TTYWidth);
    return True;
}
U0 TTYPrintC(Char c)
{
    static Bool lnfg = False;
    static Bool lnbg = False;
    static Bool lnrw = False;
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
    static Bool lbs = False;
    static Bool lds = False; // $
    static U8   ldv = 0    ;
    static U8   lbd = 0    ;
    static U8   lbp = 0    ;
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
U0 SerialPrintF(String format, ...) {
    va_list args;
    va_start(args, format);
    U8 n = 4;
    while (*format) {
        if (*format == '%' && *(format + 1)) {
            ++format;
            if (*format >= '0' && *format <= '4') {
                n = *format - '0';
                ++format;
            }
            switch (*format) {
                case 'c': {
                    Char c = va_arg(args, U32);
                    SerialWrite(c);
                } break;
                case 'C': {
                    Char c = UpperTo(va_arg(args, U32));
                    SerialWrite(c);
                } break;
                case 'b': {
                    U16 s = va_arg(args, U32);
                    for (I8 i = n * 8 - 1; i >= 0; --i) {
                        SerialWrite((s & (1 << i)) ? '1' : '0');
                    }
                } break;
                case 'p': {
                    U32 s = va_arg(args, U32);
                    SerialWrite('0');
                    SerialWrite('x');
                    for (U8 i = n * 2; i > 0; --i)
                        SerialWrite("0123456789ABCDEF"[(s >> ((i - 1) * 4)) & 0xF]);
                } break;
                case 'x': {
                    U32 s = va_arg(args, U32);
                    for (U8 i = n * 2; i > 0; --i)
                        SerialWrite("0123456789abcdef"[(s >> ((i - 1) * 4)) & 0xF]);
                } break;
                case 'X': {
                    U32 s = va_arg(args, U32);
                    for (U8 i = n * 2; i > 0; --i)
                        SerialWrite("0123456789ABCDEF"[(s >> ((i - 1) * 4)) & 0xF]);
                } break;
                case '%': {
                    SerialWrite('%');
                } break;
                default:
                    SerialWrite('%');
                    SerialWrite(*format);
            }
            n = 4;
            ++format;
            continue;
        }
        SerialWrite(*format);
        ++format;
    }
    va_end(args);
}
U0 PrintF(String format, ...) {
    va_list args;
    va_start(args, format);
    U8 n = 4;
    while (*format) {
        if (*format == '%' && *(format + 1)) {
            ++format;
            if (*format >= '0' && *format <= '4') {
                n = *format - '0';
                ++format;
            }
            switch (*format) {
                case 'c': {
                    Char c = va_arg(args, U32);
                    TTYUPrintC(c);
                } break;
                case 'C': {
                    Char c = UpperTo(va_arg(args, U32));
                    TTYUPrintC(c);
                } break;
                case 'w': {
                    WordGen();
                } break;
                case 's': {
                    String s = va_arg(args, String);
                    if (!s) s = "(null)";
                    TTYUPrint(s);
                } break;
                case 'B': {
                    Bool s = va_arg(args, U32);
                    TTYUPrint(s ? "True" : "False");
                } break;
                case 'b': {
                    U16 s = va_arg(args, U32);
                    for (I8 i = n * 8 - 1; i >= 0; --i) {
                        TTYUPrintC((s & (1 << i)) ? '1' : '0');
                    }
                } break;                
                case 'd': {
                    U32 s = va_arg(args, U32);
                    TTYUPrintDec(s);
                } break;
                case 'i': {
                    I32 s = va_arg(args, I32);
                    TTYUPrintDecI(s);
                } break;
                case 'p': {
                    U32 s = va_arg(args, U32);
                    TTYUPrint("0x");
                    TTYUPrintHex(s);
                } break;
                case 'x': {
                    U32 s = va_arg(args, U32);
                    for (U8 i = n * 2; i > 0; --i)
                        TTYUPrintC("0123456789abcdef"[(s >> ((i - 1) * 4)) & 0xF]);
                } break;
                case 'X': {
                    U32 s = va_arg(args, U32);
                    for (U8 i = n * 2; i > 0; --i)
                        TTYUPrintC("0123456789ABCDEF"[(s >> ((i - 1) * 4)) & 0xF]);
                } break;
                case '%': {
                    TTYUPrintC('%');
                } break;
                default:
                    TTYUPrintC('%');
                    TTYUPrintC(*format);
            }
            n = 4;
            ++format;
            continue;
        }
        TTYUPrintC(*format);
        ++format;
    }
    va_end(args);
}

unsigned char TTYFont[256][5] = {
    ['!'] = {0b00100, 0b00100, 0b00100, 0b00000, 0b00100},
    ['"'] = {0b01010, 0b01010, 0b00000, 0b00000, 0b00000},
    ['#'] = {0b01010, 0b11111, 0b01010, 0b11111, 0b01010},
    ['$'] = {0b00100, 0b01100, 0b01110, 0b00110, 0b00100},
    ['%'] = {0b10001, 0b00010, 0b00100, 0b01000, 0b10001},
    ['&'] = {0b01100, 0b10010, 0b01101, 0b10010, 0b01101},
    ['\''] = {0b00100, 0b00100, 0b00000, 0b00000, 0b00000},
    ['('] = {0b00100, 0b01000, 0b01000, 0b01000, 0b00100},
    [')'] = {0b00100, 0b00010, 0b00010, 0b00010, 0b00100},
    ['*'] = {0b00000, 0b00100, 0b00000, 0b00000, 0b00000},
    ['+'] = {0b00000, 0b00100, 0b01110, 0b00100, 0b00000},
    [','] = {0b00000, 0b00000, 0b00000, 0b00100, 0b01000},
    ['-'] = {0b00000, 0b00000, 0b01110, 0b00000, 0b00000},
    ['.'] = {0b00000, 0b00000, 0b00000, 0b00110, 0b00110},
    ['/'] = {0b00001, 0b00010, 0b00100, 0b01000, 0b10000},
    ['0'] = {0b01110, 0b11001, 0b10101, 0b10011, 0b01110},
    ['1'] = {0b00010, 0b00110, 0b00010, 0b00010, 0b00010},
    ['2'] = {0b01110, 0b10001, 0b00001, 0b00010, 0b01111},
    ['3'] = {0b01110, 0b00001, 0b01110, 0b00001, 0b01110},
    ['4'] = {0b00100, 0b01100, 0b11111, 0b00100, 0b00100},
    ['5'] = {0b01111, 0b01000, 0b00110, 0b00001, 0b00110},
    ['6'] = {0b00110, 0b01000, 0b01110, 0b01001, 0b01110},
    ['7'] = {0b01111, 0b00001, 0b00010, 0b00100, 0b01000},
    ['8'] = {0b01110, 0b10001, 0b01110, 0b10001, 0b01110},
    ['9'] = {0b01111, 0b01001, 0b01111, 0b00001, 0b01111},
    [':'] = {0b00000, 0b00100, 0b00000, 0b00000, 0b00100},
    [';'] = {0b00100, 0b00000, 0b00000, 0b00100, 0b01000},
    ['<'] = {0b00000, 0b01000, 0b10000, 0b01000, 0b00000},
    ['='] = {0b00000, 0b01110, 0b00000, 0b01110, 0b00000},
    ['>'] = {0b00000, 0b00010, 0b00001, 0b00010, 0b00000},
    ['?'] = {0b00110, 0b01001, 0b00001, 0b00000, 0b00001},
    ['@'] = {0b01110, 0b10101, 0b10111, 0b10000, 0b01110},
    ['A'] = {0b00100, 0b01010, 0b01110, 0b01010, 0b01010},
    ['B'] = {0b11100, 0b10010, 0b11100, 0b10010, 0b11100},
    ['C'] = {0b01110, 0b10000, 0b10000, 0b10000, 0b01110},
    ['D'] = {0b11100, 0b10010, 0b10010, 0b10010, 0b11100},
    ['E'] = {0b11110, 0b10000, 0b11110, 0b10000, 0b11110},
    ['F'] = {0b11111, 0b10000, 0b11111, 0b10000, 0b10000},
    ['G'] = {0b01110, 0b10000, 0b10111, 0b10001, 0b01110},
    ['H'] = {0b10001, 0b10001, 0b11111, 0b10001, 0b10001},
    ['I'] = {0b01110, 0b00100, 0b00100, 0b00100, 0b01110},
    ['J'] = {0b00010, 0b00010, 0b00010, 0b00010, 0b01100},
    ['K'] = {0b10010, 0b10100, 0b11000, 0b10100, 0b10010},
    ['L'] = {0b10000, 0b10000, 0b10000, 0b10000, 0b11110},
    ['M'] = {0b11011, 0b10101, 0b10001, 0b10001, 0b10001},
    ['N'] = {0b11001, 0b10101, 0b10101, 0b10011, 0b10001},
    ['O'] = {0b01110, 0b10001, 0b10001, 0b10001, 0b01110},
    ['P'] = {0b11100, 0b10010, 0b11100, 0b10000, 0b10000},
    ['Q'] = {0b01100, 0b10010, 0b11010, 0b10110, 0b01110},
    ['R'] = {0b11100, 0b10010, 0b11100, 0b11000, 0b10100},
    ['S'] = {0b01110, 0b10000, 0b01110, 0b00001, 0b01110},
    ['T'] = {0b01110, 0b00100, 0b00100, 0b00100, 0b00100},
    ['U'] = {0b10001, 0b10001, 0b10001, 0b10001, 0b01110},
    ['V'] = {0b10001, 0b10001, 0b01010, 0b01010, 0b00100},
    ['W'] = {0b10001, 0b10001, 0b10101, 0b10101, 0b01010},
    ['X'] = {0b10001, 0b01010, 0b00100, 0b01010, 0b10001},
    ['Y'] = {0b10001, 0b01010, 0b00100, 0b00100, 0b00100},
    ['Z'] = {0b11111, 0b00010, 0b00100, 0b01000, 0b11111},
    ['['] = {0b11100, 0b10000, 0b10000, 0b10000, 0b11100},
    ['\\'] = {0b10000, 0b01000, 0b00100, 0b00010, 0b00001},
    [']'] = {0b00111, 0b00001, 0b00001, 0b00001, 0b00111},
    ['^'] = {0b00100, 0b01010, 0b00000, 0b00000, 0b00000},
    ['_'] = {0b00000, 0b00000, 0b00000, 0b00000, 0b01110},
    ['`'] = {0b10000, 0b01000, 0b00000, 0b00000, 0b00000},
    ['a'] = {0b00110, 0b00001, 0b00111, 0b01001, 0b00111},
    ['b'] = {0b10000, 0b11110, 0b10001, 0b10001, 0b11110},
    ['c'] = {0b00000, 0b01110, 0b10000, 0b10000, 0b01110},
    ['d'] = {0b00001, 0b00001, 0b00111, 0b01001, 0b00111},
    ['e'] = {0b00000, 0b01110, 0b11110, 0b10000, 0b01110},
    ['f'] = {0b00110, 0b01000, 0b11110, 0b01000, 0b01000},
    ['g'] = {0b00000, 0b00011, 0b00011, 0b00001, 0b01110},
    ['h'] = {0b01000, 0b01000, 0b01100, 0b01010, 0b01010},
    ['i'] = {0b00100, 0b00000, 0b00100, 0b00100, 0b00100},
    ['j'] = {0b00000, 0b00010, 0b00010, 0b00010, 0b01100},
    ['k'] = {0b10010, 0b10100, 0b11000, 0b11000, 0b10100},
    ['l'] = {0b00100, 0b00100, 0b00100, 0b00100, 0b00100},
    ['m'] = {0b00000, 0b11010, 0b10101, 0b10001, 0b10001},
    ['n'] = {0b00000, 0b11100, 0b10010, 0b10010, 0b10010},
    ['o'] = {0b00000, 0b01100, 0b10010, 0b10010, 0b01100},
    ['p'] = {0b00000, 0b11100, 0b10100, 0b11000, 0b10000},
    ['q'] = {0b00000, 0b01110, 0b01010, 0b00110, 0b00010},
    ['r'] = {0b00000, 0b01110, 0b01001, 0b01000, 0b01000},
    ['s'] = {0b00000, 0b00110, 0b00100, 0b00010, 0b00110},
    ['t'] = {0b00100, 0b01110, 0b00100, 0b00100, 0b00010},
    ['u'] = {0b00000, 0b01001, 0b01001, 0b01001, 0b00111},
    ['v'] = {0b00000, 0b10010, 0b10010, 0b01100, 0b01100},
    ['w'] = {0b00000, 0b10001, 0b10001, 0b10101, 0b01010},
    ['x'] = {0b00000, 0b01001, 0b00110, 0b00110, 0b01001},
    ['y'] = {0b00000, 0b01010, 0b00100, 0b01000, 0b10000},
    ['z'] = {0b00000, 0b01110, 0b00010, 0b00100, 0b01110},
    ['{'] = {0b00110, 0b00100, 0b01100, 0b00100, 0b00110},
    ['|'] = {0b00100, 0b00100, 0b00100, 0b00100, 0b00100},
    ['}'] = {0b01100, 0b00100, 0b00110, 0b00100, 0b01100},
    ['~'] = {0b00000, 0b01010, 0b10100, 0b00000, 0b00000},
    [0x80] = {0b10101, 0b01010, 0b10101, 0b01010, 0b10101},
    [0x81] = {0b10101, 0b01110, 0b11111, 0b01110, 0b10101},
    [0x82] = {0b11111, 0b11111, 0b11111, 0b11111, 0b11111},
    [0x83] = {0b00000, 0b01111, 0b01000, 0b01011, 0b01010},
    [0x84] = {0b00000, 0b11111, 0b00000, 0b11011, 0b01010},
    [0x85] = {0b00000, 0b11110, 0b00010, 0b11010, 0b01010},
    [0x86] = {0b01010, 0b11010, 0b00010, 0b11110, 0b00000},
    [0x87] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0x88] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0x89] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0x8a] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0x8b] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0x8c] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0x8d] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0x8e] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0x8f] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0x90] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0x91] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0x92] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0x93] = {0b01010, 0b01011, 0b01000, 0b01011, 0b01010},
    [0x94] = {0b01010, 0b11011, 0b00000, 0b11011, 0b01010},
    [0x95] = {0b01010, 0b11010, 0b00010, 0b11010, 0b01010},
    [0x96] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0x97] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0x98] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0x99] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0x9a] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0x9b] = {0b00101, 0b01010, 0b10100, 0b01010, 0b00101},
    [0x9c] = {0b10100, 0b01010, 0b00101, 0b01010, 0b10100},
    [0x9d] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0x9e] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0x9f] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xa0] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xa1] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xa2] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xa3] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xa4] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xa5] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xa6] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xa7] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xa8] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xa9] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xaa] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xab] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xac] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xad] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xae] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xaf] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xb0] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xb1] = {0b00100, 0b01010, 0b01110, 0b10001, 0b10001},
    [0xb2] = {0b11111, 0b10000, 0b11111, 0b10001, 0b11110},
    [0xb3] = {0b11110, 0b10001, 0b11110, 0b10001, 0b11110},
    [0xb4] = {0b11111, 0b10000, 0b10000, 0b10000, 0b10000},
    [0xb5] = {0b01110, 0b01010, 0b10010, 0b11111, 0b10001},
    [0xb6] = {0b11111, 0b10000, 0b11111, 0b10000, 0b11111},
    [0xb7] = {0b10101, 0b10101, 0b01110, 0b10101, 0b10101},
    [0xb8] = {0b11111, 0b00001, 0b11110, 0b00001, 0b11111},
    [0xb9] = {0b10001, 0b10011, 0b10101, 0b11001, 0b10001},
    [0xba] = {0b00100, 0b10011, 0b10101, 0b11001, 0b10001},
    [0xbb] = {0b10010, 0b10100, 0b11000, 0b10100, 0b10010},
    [0xbc] = {0b00011, 0b00101, 0b00101, 0b01001, 0b10001},
    [0xbd] = {0b11011, 0b10101, 0b10001, 0b10001, 0b10001},
    [0xbe] = {0b10001, 0b10001, 0b11111, 0b10001, 0b10001},
    [0xbf] = {0b01110, 0b10001, 0b10001, 0b10001, 0b01110},
    [0xc0] = {0b11111, 0b10001, 0b10001, 0b10001, 0b10001},
    [0xc1] = {0b11100, 0b10010, 0b11100, 0b10000, 0b10000},
    [0xc2] = {0b01110, 0b10000, 0b10000, 0b10000, 0b01110},
    [0xc3] = {0b11111, 0b00100, 0b00100, 0b00100, 0b00100},
    [0xc4] = {0b10001, 0b10001, 0b01010, 0b00100, 0b00100},
    [0xc5] = {0b00100, 0b01110, 0b10101, 0b01110, 0b00100},
    [0xc6] = {0b10001, 0b01010, 0b00100, 0b01010, 0b10001},
    [0xc7] = {0b10010, 0b10010, 0b10010, 0b11111, 0b00001},
    [0xc8] = {0b10001, 0b10001, 0b01111, 0b00001, 0b00001},
    [0xc9] = {0b10101, 0b10101, 0b10101, 0b10101, 0b11111},
    [0xca] = {0b10101, 0b10101, 0b10101, 0b11111, 0b00001},
    [0xcb] = {0b11000, 0b01000, 0b01110, 0b01001, 0b01110},
    [0xcc] = {0b10001, 0b10001, 0b11001, 0b10101, 0b11001},
    [0xcd] = {0b10000, 0b10000, 0b11100, 0b10010, 0b11100},
    [0xce] = {0b11110, 0b00001, 0b11111, 0b00001, 0b11110},
    [0xcf] = {0b10010, 0b10101, 0b11101, 0b10101, 0b10010},
    [0xd0] = {0b00110, 0b01001, 0b00111, 0b01001, 0b10001},
    [0xd1] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xd2] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xd3] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xd4] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xd5] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xd6] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xd7] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xd8] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xd9] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xda] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xdb] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xdc] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xdd] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xde] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xdf] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xe0] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xe1] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xe2] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xe3] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xe4] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xe5] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xe6] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xe7] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xe8] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xe9] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xea] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xeb] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xec] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xed] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xee] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xef] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xf0] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xf1] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xf2] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xf3] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xf4] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xf5] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xf6] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xf7] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xf8] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xf9] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xfa] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xfb] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xfc] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xfd] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xfe] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},

};

U0 TTYPutG(Char c) {
    U32 cur = TTYCursor;
    U32 x = (cur % TTYWidth) * 6 + 0 + TTYCursorX;
    U32 y = (cur / TTYWidth) * 6 + 1 + TTYCursorY;
    VRMDrawRect(vec2(x, y), vec2(x+6, y+6), TTYlbg);
    for (U32 i = 0; i < 5; ++i) {
        for (U32 j = 0; j < 5; ++j) {
            Bool bit = (TTYFont[c][i] >> (4-j)) & 1;
            VRMPSet(x+j, y+i, bit ? TTYlfg : TTYlbg);
        }
    }
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
    
    // SerialWrite(c);
    for (U32 i = 0; i < 0x1B; ++i) {
        SerialWrite(buf[i]);
    }
}
U0 TTYScrollG() {
    MemCpy(VRM, VRM+320*6, 64000-320*6);
    for (U32 i = TTYWidth * TTYHeight - TTYWidth; i < TTYWidth * TTYHeight; ++i) {
        TTYCursor = i;
        TTYPuter(' ');
    }
}
U0 TTYScrollT() {
    MemCpy(vga, vga+80, 4000-80);
    for (U32 i = TTYWidth * TTYHeight - TTYWidth; i < TTYWidth * TTYHeight; ++i) {
        TTYCursor = i;
        TTYPuter(' ');
    }
}
U0 TTYScrollS() {

}
U0 TTYSwitch(TTYContext tc) {
    static TTYContext prev = TTYC_VGA;
    static TTYContext curr = TTYC_VGA;
    switch (tc) {
        case TTYC_VGA:
            TTYPuter = TTYPutT;
            TTYScroller = TTYScrollT;
            TTYWidth = 80;
            TTYHeight = 25;
            prev = curr;
            curr = tc;
            break;
        case TTYC_320:
            TTYPuter = TTYPutG;
            TTYScroller = TTYScrollG;
            TTYWidth = 320/6;
            TTYHeight = 200/6;
            prev = curr;
            curr = tc;
            break;
        case TTYC_RES:
            TTYSwitch(prev);
            break;
        case TTYC_SER:
            TTYPuter = TTYPutS;
            TTYScroller = TTYScrollS;
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