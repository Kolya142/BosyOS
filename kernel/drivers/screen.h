#include <lib/types.h>
#include <arch/ports.h>
#pragma once
#define VGA_ADDRESS 0xB8000
#define BACKSPACE 8
#define NL 10
#define CR 13

extern short *vga;
extern short vvga[80*25];
enum VgaColor {
    Black,
    DBlue,
    DGreen,
    DCyan,
    DRed,
    DPurple,
    Brown,
    Gray,
    DGray,
    Blue,
    Green,
    Cyan,
    Red,
    Purple,
    Yellow,
    White,
};
typedef enum VgaColor VgaColor;
extern int TTYCursor;
extern char TTYUpdateCursor;
extern U8 TTYCurrentColor;
void TTYPrintChar(U8 c);
void TTYRawPrintChar(char c);
void TTYSetCursor(U8 x, U8 y);
void TTYPrint(const char *c);
void TTYSetColor(VgaColor bg, VgaColor fg);

void TTYPrintPos(const char *c, char x, char y);
void TTYPrintCharPos(U8 c, char x, char y);
void TTYClearScreen();

void TTYCursorEnable(U8 first_row, U8 end_row);
void TTYCursorDisable();
void TTYCursorMove();

void TTYCursorUpdateDisable();
void TTYCursorUpdateEnable();

void TTYScrollUp();

inline short VGAGetChar(VgaColor back, VgaColor front, char c);
