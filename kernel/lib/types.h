#pragma once
typedef unsigned char U8;
typedef unsigned short U16;
typedef unsigned int U32;

typedef char I8;
typedef short I16;
typedef int I32;

typedef U32 reg_t;

#define hex "0123456789ABCDEF"

#define Asm __asm__ __volatile__

#define KEY_CTRL        0x80  // Ctrl
#define KEY_LSHIFT      0x81  // Left Shift
#define KEY_RSHIFT      0x82  // Right Shift
#define KEY_PTRSC       0x83  // Print Screen
#define KEY_ALT         0x84  // Alt
#define KEY_CAPSLOCK    0x85  // Caps Lock
#define KEY_F1          0x86  // F1
#define KEY_F2          0x87  // F2
#define KEY_F3          0x88  // F3
#define KEY_F4          0x89  // F4
#define KEY_F5          0x8A  // F5
#define KEY_F6          0x8B  // F6
#define KEY_F7          0x8C  // F7
#define KEY_F8          0x8D  // F8
#define KEY_F9          0x8E  // F9
#define KEY_F10         0x8F  // F10
#define KEY_NUMLOCK     0x90  // Num Lock
#define KEY_SCROLL      0x91  // Scroll Lock
#define KEY_HOME        0x92  // Home
#define KEY_UP          0x93  // Up Arrow
#define KEY_PGUP        0x94  // Page Up
#define KEY_LEFT        0x95  // Left Arrow
#define KEY_CENTER      0x96  // Center (No Movement)
#define KEY_RIGHT       0x97  // Right Arrow
#define KEY_END         0x98  // End
#define KEY_DOWN        0x99  // Down Arrow
#define KEY_PGDOWN      0x9A  // Page Down
#define KEY_INSERT      0x9B  // Insert
