#pragma once
#include <drivers/video/vga.h>
#include <lib/memory/MemLib.h>
#include <misc/multiboot.h>
#define WIDTH 640
#define HEIGHT 480
#define abs(x) ((x) < 0 ? -(x) : (x))


extern U32 *VVRM;
extern U8 *VRM;
extern U8 *VRM1;
extern U8 GCursor[]; // Graphics Cursor
extern U32 VRMColors[];

// Vga gRaphics Mode

typedef struct Vec2 {
    I32 x;
    I32 y;
} Vec2;

Vec2 vec2(I32 x, I32 y);
U0 VRMPSet(U16 x, U16 y, U8 c);
U0 VRMClear(U8 c);
U0 VRMFlush();
U0 VRMDrawLine(Vec2 start, Vec2 end, U8 c);
U0 VRMDrawRect(Vec2 start, Vec2 end, U8 c);
U0 VRMDrawSprite(Vec2 start, Vec2 size, U8 c, U8 b, U8 *sprite);