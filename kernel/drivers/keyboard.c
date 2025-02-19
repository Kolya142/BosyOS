#include <drivers/keyboard.h>

volatile KBStateS KBState;

INT_DEF(KBHandler) {
    INT_START;
    if (!(PIn(0x64) & 1)) {
        goto end;
    }
    U8 scode = PIn(0x60);
    KBState.SC = scode;
    KBState.Key = KBSCToASCIIP(scode);
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
    }
    end:
    INT_RETURN;
}

U0 KBInit() {
    IDTSet(0x21, KBHandler, 0x08, 0x8E);
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