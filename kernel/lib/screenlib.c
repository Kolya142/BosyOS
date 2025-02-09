#include <lib/screenlib.h>
#include "screenlib.h"

int other_cursor_state = 0;
U16 secondvgabuf[2000];

void PrintBinary(U8 d)
{
    TTYRawPrintChar('0');
    TTYRawPrintChar('b');
    TTYRawPrintChar("01"[d>>7]);
    TTYRawPrintChar("01"[(d>>6)%2]);
    TTYRawPrintChar("01"[(d>>5)%2]);
    TTYRawPrintChar("01"[(d>>4)%2]);
    TTYRawPrintChar("01"[(d>>3)%2]);
    TTYRawPrintChar("01"[(d>>2)%2]);
    TTYRawPrintChar("01"[(d>>1)%2]);
    TTYRawPrintChar("01"[d%2]);
    TTYCursorMove();
}

void PrintHex(U32 d)
{
    U8 *bytes = (U8*)&d;
    TTYRawPrintChar('0');
    TTYRawPrintChar('x');
    TTYRawPrintChar(hex[bytes[3]>>4]);
    TTYRawPrintChar(hex[bytes[3]&0xf]);
    TTYRawPrintChar(hex[bytes[2]>>4]);
    TTYRawPrintChar(hex[bytes[2]&0xf]);
    TTYRawPrintChar(hex[bytes[1]>>4]);
    TTYRawPrintChar(hex[bytes[1]&0xf]);
    TTYRawPrintChar(hex[bytes[0]>>4]);
    TTYRawPrintChar(hex[bytes[0]&0xf]);
    TTYCursorMove();
}

void CursorPushMove(U8 x, U8 y) {
    other_cursor_state = TTYCursor;
    TTYSetCursor(x, y);
    TTYCursorMove();
}
void CursorPop() {
    TTYCursor = other_cursor_state;
    TTYCursorMove();
}
void BufferSecondToFirst()
{
    for (int i = 0; i < 2000; i++) {
        vvga[i] = secondvgabuf[i];
        vvga[i] &= ~0xf0000;
        vvga[i] |= 0x9000;
    }
}
void BufferFirstToSecond()
{
    MemCpy(secondvgabuf, vga, 4000);
}
void BufferFlip() {
    MemCpy(vga, vvga, 4000);
}