#include "asciip.h"

const char *ASCIIPToName(U8 c)
{
    static const char *ascii_names[] = {
        "NUL", "SOH", "STX", "ETX", "EOT", "ENQ", "ACK", "BEL",
        "BS", "TAB", "LF", "VT", "FF", "CR", "SO", "SI",
        "DLE", "DC1", "DC2", "DC3", "DC4", "NAK", "SYN", "ETB",
        "CAN", "EM", "SUB", "ESC", "FS", "GS", "RS", "US",
        "Space", "!", "\"", "#", "$", "%", "&", "\'", "(", ")",
        "*", "+", ",", "-", ".", "/", "0", "1", "2", "3", "4",
        "5", "6", "7", "8", "9", ":", ";", "<", "=", ">", "?",
        "@", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J",
        "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U",
        "V", "W", "X", "Y", "Z", "[", "\\", "]", "^", "_", "`",
        "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k",
        "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v",
        "w", "x", "y", "z", "{", "|", "}", "~", "DEL"
    };

    switch (c) {
        case 0 ... 0x7f:
            return ascii_names[c];
        case 0x80:
            return "Ctrl";
        case 0x81:
            return "Left Shift";
        case 0x82:
            return "Right Shift";
        case 0x83:
            return "Print Screen";
        case 0x84:
            return "Alt";
        case 0x85:
            return "Caps Lock";
        case 0x86:
            return "F1";
        case 0x87:
            return "F2";
        case 0x88:
            return "F3";
        case 0x89:
            return "F4";
        case 0x8A:
            return "F5";
        case 0x8B:
            return "F6";
        case 0x8C:
            return "F7";
        case 0x8D:
            return "F8";
        case 0x8E:
            return "F9";
        case 0x8F:
            return "F10";
        case 0x90:
            return "Num Lock";
        case 0x91:
            return "Scroll Lock";
        case 0x92:
            return "Home";
        case 0x93:
            return "Up Arrow";
        case 0x94:
            return "Page Up";
        case 0x95:
            return "Left Arrow";
        case 0x96:
            return "Center";
        case 0x97:
            return "Right Arrow";
        case 0x98:
            return "End";
        case 0x99:
            return "Down Arrow";
        case 0x9A:
            return "Page Down";
        case 0x9B:
            return "Insert";
        case 0x9C:
            return "Delete";
        default:
            return "Unknown";
    }
}
