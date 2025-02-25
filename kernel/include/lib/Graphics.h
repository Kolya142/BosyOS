#pragma once
#include <drivers/vga.h>
#include <lib/MemLib.h>

extern U8 *vrm;

#define abs(x) ((x) < 0 ? -(x) : (x))


// Vga gRaphics Mode

typedef struct Vec2 {
    I32 x;
    I32 y;
} Vec2;

Vec2 vec2(I32 x, I32 y);
U0 VRMPSet(U16 x, U16 y, U8 c);
U0 VRMClear(U8 c);
U0 VRMDrawLine(Vec2 start, Vec2 end, U8 c);
U0 VRMDrawRect(Vec2 start, Vec2 end, U8 c);
U0 VRMDrawSprite(Vec2 start, Vec2 size, U8 *sprite);