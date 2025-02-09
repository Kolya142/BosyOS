#include "screen.h"
short *vga = (short*)VGA_ADDRESS;
short vvga[80 * 25];
int TTYCursor = 0;
char TTYUpdateCursor = 1;
U8 TTYCurrentColor = 0x0f;

void TTYPrintChar(U8 c)
{
    if (c == BACKSPACE) {
        TTYCursor--;
        vvga[TTYCursor] = 0x0f20;
        vga[TTYCursor] = 0x0f20;
        TTYCursorMove();
        return;
    }
    else if (c == NL) {
        TTYCursor -= TTYCursor%80;
        TTYCursor += 80;
        TTYCursorMove();
        return;
    }
    else if (c == CR)  {
        TTYCursor -= TTYCursor%80;
        TTYCursorMove();
        return;
    }
    else if (c == KEY_LEFT) {
        TTYCursor--;
        TTYCursorMove();
        return;
    }
    else if (c == KEY_RIGHT) {
        TTYCursor++;
        TTYCursorMove();
        return;
    }
    else if (c == KEY_UP) {
        TTYCursor-=80;
        TTYCursorMove();
        return;
    }
    else if (c == KEY_DOWN) {
        TTYCursor+=80;
        TTYCursorMove();
        return;
    }
    // if (TTYCursor/80 < (TTYCursor+1)/80) {
    //     vvga[TTYCursor] = VGAGetChar(White, Black, '/'); // say: is line break
    //     vga[TTYCursor] = vvga[TTYCursor];
    //     TTYCursor++;
    // }
    vvga[TTYCursor] = (TTYCurrentColor << 8) | c;
    vga[TTYCursor] = vvga[TTYCursor];
    TTYCursor++;
    TTYCursorMove();
    if (TTYCursor >= 80*24) {
        TTYScrollUp();
    }
}

void TTYRawPrintChar(char c)
{
    vvga[TTYCursor] = (TTYCurrentColor << 8) | c;
    vga[TTYCursor] = vvga[TTYCursor];
    TTYCursor++;
}

void TTYSetCursor(U8 x, U8 y)
{
    TTYCursor = x+y*80;
}

void TTYPrint(const char *c)
{
    for (int i = 0; c[i]; i++) {
        TTYPrintChar(c[i]);
    }
}

void TTYSetColor(VgaColor bg, VgaColor fg)
{
    TTYCurrentColor = (bg << 4) | fg;
}

void TTYPrintPos(const char *c, char x, char y)
{
    int last_cursor = TTYCursor;
    TTYSetCursor(x, y);
    TTYPrint(c);
    TTYCursor = last_cursor;
    TTYCursorMove();
}

void TTYPrintCharPos(U8 c, char x, char y)
{
    int last_cursor = TTYCursor;
    TTYSetCursor(x, y);
    TTYPrintChar(c);
    TTYCursor = last_cursor;
    TTYCursorMove();
}

void TTYClearScreen()
{
    TTYCursorEnable(0, 25);
    TTYCursor = 0;
    for (int i = 0; i < 2000; i++) {
        vga[i] = 0x0f20;
        vvga[i] = 0x0f20;
    }
}

void TTYCursorEnable(U8 first_row, U8 end_row)
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | first_row);

	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | end_row);
}

void TTYCursorDisable()
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

void TTYCursorMove()
{
    if (!TTYUpdateCursor) {
        return;
    }
	outb(0x3D4, 0x0F);
	outb(0x3D5, (U8) (TTYCursor & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (U8) ((TTYCursor >> 8) & 0xFF));
}

void TTYCursorUpdateDisable()
{
    TTYUpdateCursor = 0;
}

void TTYCursorUpdateEnable()
{
    TTYUpdateCursor = 1;
}

short VGAGetChar(VgaColor back, VgaColor front, char c)
{
    return ((back<<4)|(front))<<8 | c;
}


void TTYScrollUp() {
    for (int i = 0; i < 80*25; i++) {
        vga[i] = 0x0f20;
    }
    for (int i = 0; i < 80*24; i++) {
        vga[i] = vvga[i+80];
        vvga[i] = vga[i];
    }
    for (int i = 80*24; i < 80*25; i++) {
        vga[i] = 0x0f20;
    }
    TTYCursor -= 80;
    TTYCursorMove();
}