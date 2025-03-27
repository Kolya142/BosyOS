#include <lib/graphics/Graphics.h>
#include <drivers/time/pit.h>
Bool VRMState = True;
U32 *VVRM;
U8 *VRM = (U8*)0x300000+(WIDTH*HEIGHT)*0;
U8 *VRM1 = (U8*)0x300000+(WIDTH*HEIGHT)*1;
U32 VRMColors[] = {
    0x000000,
    0x000055,
    0x005500,
    0x005555,
    0x550000,
    0x550055,
    0x554400,

    0x565656,

    0x444444,

    0x0000FF,
    0x00FF00,
    0x00FFFF,
    0xFF0000,
    0xFF00FF,
    0xF5D400,
    0xFFFFFF,
};
U8 GCursor[] = {
    1,1,2,2,2, 2,2,2,2,2,
    1,0,1,2,2, 2,2,2,2,2,
    1,0,0,1,2, 2,2,2,2,2,
    1,0,0,0,1, 2,2,2,2,2,
    1,0,0,0,1, 2,2,2,2,2,

    1,0,0,1,2, 2,2,2,2,2,
    1,0,1,1,2, 2,2,2,2,2,
    1,1,2,2,1, 2,2,2,2,2,
    2,2,2,2,2, 1,2,2,2,2,
    2,2,2,2,2, 2,1,2,2,2,
};
Vec2 vec2(I32 x, I32 y)
{
    return (Vec2) {.x = x, .y = y};
}
U0 VRMPSet(U16 x, U16 y, U8 c) {
    if (x >= WIDTH || y >= HEIGHT) return;
    VRM[y * WIDTH + x] = c;
}
U0 VRMClear(U8 c) {
    MemSet(VRM, c, WIDTH*HEIGHT);
}
U0 VRMFlush() {
    for (U32 i = 0; i < WIDTH * HEIGHT; ++i) {
        U8 c = VRM1[i] != 0xFF ? VRM1[i] : VRM[i];
        if (c > sizeof(VRMColors) / sizeof(VRMColors[0])) {
            continue;
        }
        VVRM[i] = VRMColors[c];
    }
}

U0 VRMDrawLine(Vec2 start, Vec2 end, U8 c) {
    I32 dx = abs(end.x - start.x);
    I32 dy = abs(end.y - start.y);
    I32 sx = (start.x < end.x) ? 1 : -1;
    I32 sy = (start.y < end.y) ? 1 : -1;
    I32 err = dx - dy;

    I32 x = start.x;
    I32 y = start.y;

    while (1) {
        VRMPSet(x, y, c);
        if (x == end.x && y == end.y) break;
        I32 e2 = err * 2;
        if (e2 > -dy) { err -= dy; x += sx; }
        if (e2 < dx) { err += dx; y += sy; }
    }
}
U0 VRMDrawRect(Vec2 start, Vec2 end, U8 c) {
    for (U16 x = start.x; x < end.x; ++x) {
        for (U16 y = start.y; y < end.y; ++y) {
            VRMPSet(x, y, c);
        }
    }
}
U0 VRMDrawSprite(Vec2 start, Vec2 size, U8 c, U8 b, U8 *sprite) {
    for (U32 y = 0; y < size.y; ++y) {
        for (U32 x = 0; x < size.x; ++x) {
            U8 s = sprite[x + y * size.x];
            if (s == 2) continue;
            VRMPSet(x + start.x, y + start.y, s == 1 ? c: b);
        }
    }
}