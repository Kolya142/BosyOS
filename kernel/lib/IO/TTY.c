#include <lib/IO/TTY.h>

U32 TTYCursor = 0;
U32 TTYGSX = 0;
U32 TTYGSY = 0;
PTerm VTerm;
TTY TTerm;

U0 TTYInit() {
    VTerm = PTermInit(2048);
    TTerm.width = 320 / 6;
    TTerm.height = 200 / 6;
    TTerm.render = TTYRenderG;
}
U0 TTYClear() {
    TTYCursor = 0;
    PTermWrite(&VTerm, 1, (Char[]){ ASCIIPCtrl }, 1);
    TTerm.render();
}
U0 TTYRawPrint(Char c, VgaColor fg, VgaColor bg) {
    TTYPrintC(c);
}

inline U0 TTYPrintC(Char c) {
    while (!PTermWrite(&VTerm, 1, &c, 1)) {
        TTerm.render();
    }
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
    TTerm.render();
    Ptr rend = TTerm.render;
    TTerm.render = TTYRenderS;
    PrintF(format);
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

U32 KBTimeout = 500;
U32 KBRate = 200;

static Bool lk[256] = {0};
static U32 bufferi = 0;
static U32 time = 0;
static U32 timea = 0;
static U8 buf[2048];

U0 TTYInput() {
    for (U16 key = 0; key < 256; ++key) {
        if (KBState.keys[key]) {
            if (!lk[key] || ((PITTime - time >= KBTimeout) && (PITTime - timea >= KBRate))) {
                if (!lk[key]) {
                    time = PITTime;
                }
                else {
                    timea = PITTime;
                }
                lk[key] = True;
                if (key == '\b') {
                    if (bufferi) {
                        buf[bufferi--] = 0;
                        TTYUPrintC('\b');
                        TTerm.render();
                    }
                }
                else if (key == '\x1b') return;
                else if (key == '\r') {
                    TTYUPrintC('\n');
                    TTerm.render();
                    PTermWrite(&VTerm, 0, buf, bufferi);
                    bufferi = 0;
                }
                else if (key < 0x80 || (key >= 0xB1 && key <= 0xD0)) {
                    if (KBState.Shift && !(key >= 0xB1 && key <= 0xD0)) {
                        key = UpperTo(key);
                    }
                    if (bufferi < 2048 - 1) {
                        buf[bufferi++] = key;
                        TTYUPrintC(key);
                        TTerm.render();
                    }
                }
            }
        }
        else if (!KBState.keys[key] && lk[key]) {
            lk[key] = False;
        }
    }
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

U0 TTYRenderGS() {
    U32 cc = 0;
    U32 y = TTYGSY;
    for (Char c;PTermRead(&VTerm, 1, &c, 1);) {
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
    for (Char c;PTermRead(&VTerm, 1, &c, 1);) {
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
        if (raw) {
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
                PTermRead(&VTerm, 1, &c, 1);
                fg = c - ASCIIPCBlack;
                break;
            }
            case ASCIIPF2: {
                PTermRead(&VTerm, 1, &c, 1);
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
U0 TTYRenderT() {
    static U32 fg = White;
    static U32 bg = Black;
    static Bool raw = False;
    for (Char c;PTermRead(&VTerm, 1, &c, 1);) {
        if (TTYCursor >= TTerm.width*TTerm.height) {
            MemCpy(vga, vga+80, 4000-80);
            for (U32 i = 80 * 25 - 80; i < 80 * 25; ++i) {
                vga[i] = 0;
            }
            TTYCursor = TTerm.width * (TTerm.height - 1);
        }
        if (raw) {
            U32 x = (TTYCursor % TTerm.width);
            U32 y = (TTYCursor / TTerm.width);
            VgaPSet(x, y, c, VgaColorGet(fg, bg));
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
                U32 x = (TTYCursor % TTerm.width);
                U32 y = (TTYCursor / TTerm.width);
                VgaPSet(x, y, ' ', VgaColorGet(fg, bg));
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
                PTermRead(&VTerm, 1, &c, 1);
                fg = c - ASCIIPCBlack;
                break;
            }
            case ASCIIPF2: {
                PTermRead(&VTerm, 1, &c, 1);
                bg = c - ASCIIPCBlack;
                break;
            }
            case ASCIIPCtrl: {
                while (TTYCursor < TTerm.width * TTerm.height) {
                    vga[TTYCursor] = 0;
                }
                TTYCursor = 0;
            }
            case ASCIIPNextRaw: {
                raw = True;
                break;
            }
            default: {
                U32 x = (TTYCursor % TTerm.width);
                U32 y = (TTYCursor / TTerm.width);
                VgaPSet(x, y, c, VgaColorGet(fg, bg));
                ++TTYCursor;
                break;
            }
        }
    }
}
U0 TTYRenderS() {
    for (Char c;PTermRead(&VTerm, 1, &c, 1);) {
        if (c == ASCIIPCtrl) {
            SerialWrite('\x1b');SerialWrite('[');SerialWrite('2');SerialWrite('J');
            SerialWrite('\x1b');SerialWrite('[');SerialWrite('H');
        }
        SerialWrite(c);
    }
}