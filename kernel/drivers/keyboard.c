#include <drivers/keyboard.h>
#include <misc/driverreg.h>
#include <lib/TTY.h>

volatile KBStateS KBState;
static U8 layout;
extern U8 MouseCycle;

INT_DEF(KBHandler) {
    if (!(PIn(0x64) & 1)) {
        return;
    }
    U8 scode = PIn(0x60);
    KBState.SC = scode;
    KBState.Key = KBSCToASCIIP(scode);
    if (KBState.Key == ' ' && !(KBState.SC & 0x80) && KBState.Super) {
        layout = (layout+1)%2;
        return;
    }
    switch (layout)
    {
        case 0:
            // do nothing
        break;
        case 1:
            KBState.Key = KBKeyToRus(KBState.Key);
        break;
    }
    if (scode == 0xE0) {
        U8 ext = PIn(0x60);
        if (ext == 0x5B || ext == 0x5C) {
            KBState.Super = True;
        }
        else if (ext == 0xDB || ext == 0xDC) {
            KBState.Super = False;
        }
    }
    if (scode & 128) {
        scode &= ~128;
        if (scode == 42) {
            KBState.Shift = False;
        }
        else if (scode == 54) {
            KBState.Shift = False;
        }
        else if (scode == 29) {
            KBState.Ctrl = False;
        }
        KBState.keys[KBState.Key] = False;
    }
    else {
        if (scode == 42) {
            KBState.Shift = True;
        }
        else if (scode == 54) {
            KBState.Shift = True;
        }
        else if (scode == 29) {
            KBState.Ctrl = True;
        }
        KBState.keys[KBState.Key] = True;
    }
}
static U0 KBDriverHandler(U32 id, U32 *value) {
    switch (id)
    {
        case 0:
            *value = KBState.Key;
            break;
        case 1:
            *value = KBState.SC;
            break;
        case 2:
            *value = (KBState.Ctrl << 2) | (KBState.Shift << 1) | KBState.Super;
            break;
        case 3:
            *value = KBState.keys[*value];
            break;
    }
}
U0 KBInit() {
    DriverReg(0x0bb5676c, 0xff3ae302, KBDriverHandler, "keyboard");
    IDTSet(33, KBHandler, 0x08, 0x8E);
}
U8 KBSCToASCIIP(U8 code)
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
U8 KBKeyToRus(U8 code)
{
    if (code == '[') return '\xC6';
    if (code == ']') return '\xCB';
    if (code == ';') return '\xB7';
    if (code == '\'') return '\xCE';
    if (code == ',') return '\xB2';
    if (code == '.') return '\xCF';
    if (code == '`') return '\xB6';
    if (code >= 'A' && code <= 'Z') code = code - 'A' + 'a';
    if (code <= 'a' || code >= 'z') return code;
    return "\xc5\xb9\xc2\xb3\xc4\xb1\xc0\xc1\xc9\xbf\xbc\xb5\xcd\xc3\xca\xb8\xba\xbb\xcc\xb6\xb4\xbd\xc7\xc8\xbe\xd0"
        [code - 'a'];
}
