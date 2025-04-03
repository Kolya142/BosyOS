#include <lib/strings/String.h>
#include <lib/IO/TTY.h>

U0 TTYPrintC(U32 tty, Char c) {
    TTYWrite(tty, 1, &c, 1);
}

U0 TTYUPrintC(U32 tty, Char c) {
    static Bool lbs = False;
    static Bool lds = False; // $
    static U8   ldv = 0    ;
    static U8   lbd = 0    ;
    static U8   lbp = 0    ;
    if (lbs) {
        if (c == '\\') 
            TTYPrintC(tty, '\\');
        if (c == 'n')
            TTYPrintC(tty, '\n');
        if (c == '$') 
            TTYPrintC(tty, '$'); 
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
            TTYPrintC(tty, (AsciiP)ASCIIPUp);
            lds = 0;
        }
        else if (c == 'D') {
            TTYPrintC(tty, (AsciiP)ASCIIPDown);
            lds = 0;
        }
        else if (c == 'L') {
            TTYPrintC(tty, (AsciiP)ASCIIPLeft);
            lds = 0;
        }
        else if (c == 'R') {
            TTYPrintC(tty, (AsciiP)ASCIIPRight);
            lds = 0;
        }
        else if (c == 'H') {
            TTYPrintC(tty, (AsciiP)ASCIIPHome);
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
                TTYPrintC(tty, (AsciiP)ASCIIPNextRaw);
                TTYPrintC(tty, lbd);
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
                        TTYPrintC(tty, (AsciiP)ASCIIPF1);
                        TTYPrintC(tty, d+(AsciiP)ASCIIPCBlack);
                    break;
                    case 2:
                        TTYPrintC(tty, (AsciiP)ASCIIPF2);
                        TTYPrintC(tty, d+(AsciiP)ASCIIPCBlack);
                    break;
                    case 3:
                        TTYPrintC(tty, (AsciiP)ASCIIPF3);
                        TTYPrintC(tty, d+(AsciiP)ASCIIPCBlack);
                    break;
                    default:
                        TTYPrintC(tty, '$');
                        TTYPrintC(tty, c);
                    break;
                }
            }
            ldv = 0;
            lds = False;
        }
    }
    else if (c & 0x80) {
        TTYPrintC(tty, (AsciiP)ASCIIPNextRaw);
        TTYPrintC(tty, c);
    }
    else if (c == '\\') {lbs = True;}
    else {
        if (c == '$') {
            lds = True;
        } 
        else {
            TTYPrintC(tty, c);
        }
    }
}

U0 TTYUPrintDec(U32 tty, U32 a) {
    if (!a) {
        TTYUPrintC(tty, '0');
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
            TTYUPrintC(tty, buf[i]);
        }
    }
}
U0 TTYUPrintDecI(U32 tty, I32 a) {
    if (a < 0) {
        TTYUPrintC(tty, '-');
        a = -a;
    }
    TTYUPrintDec(tty, a);
}

U0 VPrintF(U32 tty, String format, va_list args) {
    U32 n = 4;
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
                    TTYUPrintC(tty, c);
                } break;
                case 'C': {
                    Char c = UpperTo(va_arg(args, U32));
                    TTYUPrintC(tty, c);
                } break;
                case 's': {
                    String s = va_arg(args, String);
                    if (!s) s = "(null)";
                    for (U32 i = 0; s[i]; ++i) {
                        TTYUPrintC(tty, s[i]);
                    }
                } break;
                case 'B': {
                    Bool s = va_arg(args, U32);
                    if (s) {
                        TTYUPrintC(tty, 'T');
                        TTYUPrintC(tty, 'r');
                        TTYUPrintC(tty, 'u');
                        TTYUPrintC(tty, 'e');
                    }
                    else {
                        TTYUPrintC(tty, 'F');
                        TTYUPrintC(tty, 'a');
                        TTYUPrintC(tty, 'l');
                        TTYUPrintC(tty, 's');
                        TTYUPrintC(tty, 'e');
                    }
                } break;
                case 'b': {
                    U16 s = va_arg(args, U32);
                    for (I8 i = n * 8 - 1; i >= 0; --i) {
                        TTYUPrintC(tty, (s & (1 << i)) ? '1' : '0');
                    }
                } break;                
                case 'd': {
                    U32 s = va_arg(args, U32);
                    TTYUPrintDec(tty, s);
                } break;
                case 'i': {
                    I32 s = va_arg(args, I32);
                    TTYUPrintDecI(tty, s);
                } break;
                case 'p': {
                    U32 s = va_arg(args, U32);
                    TTYUPrintC(tty, '0');
                    TTYUPrintC(tty, 'x');
                    for (U8 i = 8; i > 0; --i)
                        TTYUPrintC(tty, "0123456789ABCDEF"[(s >> ((i - 1) * 4)) & 0xF]);
                } break;
                case 'z': {
                    U32 s = va_arg(args, U32);
                } break;
                case 'x': {
                    U32 s = va_arg(args, U32);
                    for (U8 i = n * 2; i > 0; --i)
                        TTYUPrintC(tty, "0123456789abcdef"[(s >> ((i - 1) * 4)) & 0xF]);
                } break;
                case 'X': {
                    U32 s = va_arg(args, U32);
                    for (U8 i = n * 2; i > 0; --i)
                        TTYUPrintC(tty, "0123456789ABCDEF"[(s >> ((i - 1) * 4)) & 0xF]);
                } break;
                case '%': {
                    TTYUPrintC(tty, '%');
                } break;
                default:
                    TTYUPrintC(tty, '%');
                    TTYUPrintC(tty, *format);
            }
            n = 4;
            ++format;
            continue;
        }
        TTYUPrintC(tty, *format);
        ++format;
    }
    TTYFlush(tty);
}
U0 PrintF(String format, ...) {
    va_list args;
    va_start(args, format);
    VPrintF(TTYCurrent, format, args);
    va_end(args);
}
U0 SerialPrintF(String format, ...) {
    va_list args;
    va_start(args, format);
    VPrintF(0, format, args);
    // TTYUPrintC(0, '\n');
    va_end(args);
}