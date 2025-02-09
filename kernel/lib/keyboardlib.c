#include "keyboardlib.h"

char KeyboardLibLastKeyState = 1;
int KeyboardLibKeyTimer = 0;
U8 KeyboardLibLastKey = 0;

int KeyboardRead(char *buf, int count) {
    U8 c = 0;
    int pos;
    for (pos = 0; pos < count; pos++) {
        KeyboardAsk();
        U8 code = KeyboardCode;
        c = KeyboardKey;
        if ((c == KEY_LSHIFT || c == KEY_RSHIFT) && c != KeyboardLibLastKey) {
            KeyboardLibLastKey = c;
        }
        if (c == '\r') {
            if (!(code & 128) && KeyboardLibLastKeyState) {
                KeyboardLibLastKeyState = (code & 128);
                break;
            }
            KeyboardLibLastKeyState = (code & 128);
            pos--;
            continue;
        }
        if (c == KEY_LSHIFT || c == KEY_RSHIFT) {
            pos--;
            continue;
        }
        if (KeyboardShift) {
            c = ToUpper(c);
        }
        KeyboardLibLastKey = c;
        if (code & 128) {
            KeyboardLibLastKeyState = code & 128 ? 1 : 0;
            pos--;
            continue;
        }
        if (!KeyboardLibLastKeyState) {
            KeyboardLibKeyTimer += 1;
            int timer = KeyboardLibKeyTimer / 4;
            if (!(timer >= 900000 && timer % 100000 == 0)) {
                pos--;
                continue;
            }
        }
        else {
            KeyboardLibKeyTimer = 0;
        }
        KeyboardLibLastKeyState = code & 128 ? 1 : 0;
        if (c == BACKSPACE) {
            if (pos != 0) {
                buf[pos-1] = 0;
                pos -= 2;
            }
            if (pos < 0) {
                pos = 0;
            }
        }
        else if (c == KEY_LEFT) {
            // if (pos > 1) {
            //     pos -= 2;
            //     TTYPrintChar(c);
            // }
            // else {
            //     pos = 0;
            // }
        }
        else if (c == KEY_RIGHT) {
            // if (pos < count - 1) {
            //     pos++;
            //     TTYPrintChar(c);
            // }
        }
        else if (c == KEY_UP || c == KEY_DOWN) {
        }
        else {
            buf[pos] = c;
        }
        TTYPrintChar(c);
    }
    return pos;
}