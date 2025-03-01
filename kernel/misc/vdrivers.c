#include <lib/KeyboardLib.h>
#include <misc/vdrivers.h>
#include <lib/Random.h>
#include <lib/TTY.h>

static U0 VRandHandler(U32 id, U32 *value) {
    switch (id) {
        case 0:
            *value = RandomU();
            break;
        case 1:
            *(U16*)value = (U16)RandomU();
            break;
        case 2:
            *(U8*)value = (U8)RandomU();
            break;
        default:
            *value = 0;
            break;
    }
}
static U0 VTTYHandler(U32 id, U32 *value) {
    if (id == 0) {
        TTYUPrint((String)value);
    }
    else if (id == 1) {
        KBRead((String)&value[1], value[0]);
    }
    else if (id == 2) { // Give printf function
        *value = (U32)PrintF;
    }
    else if (id == 3) {
        TTYUPrintC((Char)value);
    }
    else if (id == 4) {
        *value = KBState.keys[(Char)value];
    }
    else {
        *value = 0;
    }
}

U0 VDriversReg() {
    DriverReg(0x8e6d1237, 0xc02dd685, VRandHandler);
    DriverReg(0xc0d05943, 0x8be5d999, VTTYHandler);
}