#include "kerboard.h"

char KeyboardShift = 0;
char KeyboardCtrl = 0;
U8 KeyboardKey = 0;
U8 KeyboardCode = 0;


void WriteKeyboardKey(U8 key, U8 c)
{
    const char *keyboard_char_name = ASCIIPToName(key);
    for (int i = 69; i < 80; i++) {
        vga[i] = 0x9f20;
        vvga[i] = 0x9f20;
    }
    vga[68] = (0x9f << 8) | (c & 128 ? '-' : '+');
    TTYCursorUpdateDisable();
    U32 prev_color = TTYCurrentColor;
    TTYSetColor(Blue, Yellow);
    TTYPrintPos(keyboard_char_name, 80-StrLen(keyboard_char_name), 0);
    TTYCurrentColor = prev_color;
    TTYCursorUpdateEnable();
    TTYCursorMove();
}

// void KeyboardInit()
// {
//     GUIDM id; // ab670b1e-826d8e23
//     id.dd = 0xab67;
//     id.dm = 0x0b1e;
//     id.de = 0x826d;
//     id.df = 0x8e23;

//     DriverAdd(id, KeyboardAsk, 0x08, 0x8e);
// }

void KeyboardAsk()
{
    if (!(inb(0x64) & 1))
        return;
    U8 scode = inb(0x60);
    if (scode & 128) {
        if ((scode & ~128) == 42) {
            KeyboardShift = 0;
        }
        else if ((scode & ~128) == 54) {
            KeyboardShift = 0;
        }
        else if ((scode & ~128) == 29) {
            KeyboardCtrl = 0;
        }
    }
    else {
        if (scode == 42) {
            KeyboardShift = 1;
        }
        else if (scode == 54) {
            KeyboardShift = 1;
        }
        else if (scode == 29) {
            KeyboardCtrl = 1;
        }
    }
    KeyboardCode = scode;
}

U8 KeyboardScanCodeToKey(U8 code)
{
    code &= ~0b10000000;
    code -= 1;
    return "\x1b"          // 00: esc
           "1234567890"    // 01-0A: digits
           "-=\x08\x09"    // 0B-0E: special, backspace, tab
           "qwertyuiop"    // 0F-18: letters
           "[]"            // 19-1A: square brackets
           "\r\x80"        // 1B-1C: enter, ctrl
           "asdfghjkl"     // 1D-25: letters
           ";'`"           // 26-28: special
           "\x81\\"        // 29-2A: left shift, backslash
           "zxcvbnm"       // 2B-31: letters
           ",./"           // 32-34: special
           "\x82\x83\x84"  // 35-37: right shift, print screen, alt
           " "             // 38: space
           "\x85"          // 39: caps lock
           "\x86\x87\x88\x89\x8A\x8B\x8C\x8D\x8E\x8F" // 3A-43: F1-F10
           "\x90\x91\x92"  // 44-46: num lock, scroll lock, home
           "\x93\x94"      // 47-48: up, page up
           "-"             // 49: minus
           "\x95\x96\x97"  // 4A-4C: left, center, right
           "+"             // 4D: plus
           "\x98"          // 4E: end
           "\x99\x9A"      // 4F-50: down, page down
           "\x9B\x7F"     // 51-52: insert, delete
        [code];
}
