#include <lib/IO/TTY.h>

U0 TTYInit() {
    for (U8 i = 0; i < 4; ++i) {
        VTerms[i] = PTermInit(2048);
        TTYBuffers[i] = MAlloc(2048);
    }
    VTerm = &VTerms[0];
    TTerm.width = 320 / 6;
    TTerm.height = 200 / 6;
    TTerm.render = TTYRenderG;
}
U0 TTYClear() {
    TTerm.render();

    TTYCursor = 0;
    PTermWrite(VTerm, 1, (Char[]){ ASCIIPCtrl }, 1);
    TTerm.render();

    MemSet(TTYBuffers[TTermID], 0, sizeof(TTYBuffers[TTermID]));
    TTYBuffersIndex[TTermID] = 0;
}
U0 TTYRawPrint(Char c, VgaColor fg, VgaColor bg) {
    TTYPrintC(c);
}

inline U0 TTYPrintC(Char c) {
    if (TTerm.render == TTYRenderG) { // TODO: make it more abstract
        TTYBuffers[TTermID][TTYBuffersIndex[TTermID]] = c;
        TTYBuffersIndex[TTermID] = (TTYBuffersIndex[TTermID] + 1) % 2048;
    }
    while (!PTermWrite(VTerm, 1, &c, 1)) {
        TTerm.render();
    }
}

U0 TTYUPrintC(Char c) {
    static Bool lbs = False;
    static Bool lds = False; // $
    static U8   ldv = 0    ;
    static U8   lbd = 0    ;
    static U8   lbp = 0    ;
    if (!TTYCanonical) {
        TTYPrintC(c);
        return;
    }
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
        else if (c == '@') {
            ldv = 3;
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
                    case 3:
                        TTYPrintC((AsciiP)ASCIIPF3);
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
    TTerm.render();
    Ptr rend = TTerm.render;
    TTerm.render = TTYRenderS;
    va_list args;
    va_start(args, format);
    VPrintF(format, args);
    va_end(args);
    TTYUPrintC('\n');
    TTerm.render();
    TTerm.render = rend;
}
U0 TTYUPrint(String s) {
    while (*s) {
        TTYUPrintC(*s);
        ++s;
    }
    TTerm.render();
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
    Char buf[11] = {0};
    U32 p = 10;
    buf[p] = 0;

    while (a) {
        buf[--p] = '0' + (a % 10);
        a /= 10;
    }

    for (U32 i = 0; i < 11; ++i) {
        if (buf[i]) {
            TTYUPrintC(buf[i]);
        }
    }
}
U0 TTYUPrintDecI(I32 a) {
    if (a < 0) {
        TTYUPrintC('-');
        a = -a;
    }
    TTYUPrintDec(a);
}

U0 VPrintF(String format, va_list args) {
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
                case 'z': {
                    U32 s = va_arg(args, U32);
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
}

U0 PrintF(String format, ...) {
    va_list args;
    va_start(args, format);
    VPrintF(format, args);
    va_end(args);
}
