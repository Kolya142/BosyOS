#include <drivers/video/vesa.h>
#include <kernel/KPanic.h>
#include <lib/IO/TTY.h>

static volatile U8 *vmem = (Ptr)0xFD000000;

U0 VESAInit() {
    for (U32 i = 0; i < 800*600; ++i) {
        VESAPutPixel(i % 800, i / 800, Blue);
    }
}

U0 VESAPutPixel(U16 x, U16 y, U8 color) {
    U32 p = x + y * 800;
    U32 byte = p / 2;
    U32 bit = (p % 2) * 4;
    vmem[byte] &= ~(0xF << bit);
    vmem[byte] |= (color & 0xF) << bit;
}
U0 VESAClearScreen() {
    U8 *vmem = (Ptr)0xFD000000;
    for (U32 i = 0; i < 800*600/2; ++i) {
        vmem[i] = 0;
    }
}