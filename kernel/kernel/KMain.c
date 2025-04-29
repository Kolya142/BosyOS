#include <drivers/controllers/ps2.h>
#include <drivers/input/keyboard.h>
#include <drivers/input/mouse.h>
#include <lang/Interpretter.h>
#include <drivers/time/pit.h>
#include <arch/i386/gdt.h>
#include <kernel/KHeap.h>
#include <misc/screen.h>
#include <lib/Window.h>
#include <lib/String.h>
#include <lib/MemLib.h>
#include <lib/Types.h>
#include <multiboot.h>

U0 KernelMain(struct MultiBoot *mbi) {
    mb = mbi;

    U32 mem = 0;

    {
        Ptr ptr = (Ptr)((U32)mb)+8;
        for (;;) {
            struct MultiBootTag *tag = ptr;
            if (tag->type == 0) {
                break;
            }
            if (tag->type == 8) {
                struct {
                    U32 type;
                    U32 size;
                    uint64_t addr;
                    U32 pitch;
                    U32 width;
                    U32 height;
                    U8 bpp;
                    U8 fb_type;
                    U16 reserved;
                } __attribute__((packed)) *fb = (Ptr)tag;
                fb0.width = fb->width;
                fb0.height = fb->height;
                fb0.buffer = (U32*)(U32)fb->addr;
                fb0.pitch = fb->pitch;
                fb0.bpp = fb->bpp;
            }
            ptr += (tag->size + 7) & ~7;
        }
    }
    KHeapInit();
    GDTInit();
    IDTInit();
    PITInit();
    PICMap();
    PS2Init();
    MouseInit();
    KBInit();
    U32 prev = 0;
    U32 *_gr = KMAlloc(WIDTH * HEIGHT * 4);
    U32 *_grp = KMAlloc(WIDTH * HEIGHT * 4);

    U32 *_gr0 = KMAlloc(WIDTH * HEIGHT * 4);
    Gr gr0 = (Gr) {
        .buffer = _gr0,
        .width = WIDTH,
        .height = HEIGHT
    };
    U32 *_gr1 = KMAlloc(WIDTH * HEIGHT * 4);
    for (U32 i = 0; i < WIDTH*HEIGHT; ++i) {
        _gr1[i] = 0xFF000000;
    }
    Gr gr1 = (Gr) {
        .buffer = _gr1,
        .width = WIDTH,
        .height = HEIGHT
    };
    Window winpool[] = {
        WindowInit(),
        WindowInit(),
        WindowInit(),
    };
    Window *winlist[] = {
        &winpool[0],
        &winpool[1],
        &winpool[2],
    };
    winpool[0].x = 0;
    winpool[0].y = 0;
    winpool[0].z = -4;
    winpool[0].width = gr0.width / 8;
    winpool[0].height = 10;
    winpool[0].title = "Manager";
    winpool[0].flags &= ~(1 | 2);
    winpool[1].x = 0;
    winpool[1].y = 11;
    winpool[1].z = 2;
    winpool[2].x = 50;
    winpool[2].y = 11;
    winpool[2].z = 2;
    winpool[2].red = True;
    WindowPrint(&winpool[1], "INPUT: $P$BBlinking$B");
    Bool down[256] = {0};
    for (;;) {
        String line = WindowGetLine(&winpool[1]);
        MemSet(winpool[2].text_buffer, 0, 4096);
        MemCpy(winpool[2].text_buffer, line, StrLen(line));
        KMFree(line);
        MemSet(winpool[0].text_buffer, 0, 4096);
        Char key = KBState.Key;
        if (KBState.Super) {
            WindowPrint(&winpool[0], "Super ");
        }
        if (KBState.Ctrl) {
            WindowPrint(&winpool[0], "Ctrl ");
        }
        if (KBState.Keys[ASCIIPAlt]) {
            WindowPrint(&winpool[0], "Alt ");
        }
        if (KBState.Shift) {
            WindowPrint(&winpool[0], "Shift ");
        }
        if (key == '\r') {
            WindowPrint(&winpool[0], "Return");
        }
        else if (key == '\b') {
            WindowPrint(&winpool[0], "BackSpace");
        }
        else if (!(key & 0x80)) {
            WindowPrint(&winpool[0], (Char[]) {key});
        }
        else if (key == ASCIIPPgup) {
            WindowPrint(&winpool[0], "PgUp");
        }
        else if (key == ASCIIPPgdown) {
            WindowPrint(&winpool[0], "PgDown");
        }
        else if (key == ASCIIPCapsLock) {
            WindowPrint(&winpool[0], "CapsLock");
        }
        WindowPrint(&winpool[0], "\n$BMouse position$B: $fb$fr");
        WindowPrintNum(&winpool[0], MouseX);
        WindowPrint(&winpool[0], "$fb$fr, $fr$fb");
        WindowPrintNum(&winpool[0], MouseY);
        WindowPrint(&winpool[0], "$fr$fb\n$BTime from boot$B: ");
        WindowPrintNum(&winpool[0], PITTime);
        U32 st = PITTime;
        for (U32 i = 0; i < gr0.width * gr0.height; ++i) {
            gr0.buffer[i] = 0xFFFFFF;
        }
        WindowsUpd(&gr0, winlist, 3);

        MemCpy(_gr, _gr0, WIDTH*HEIGHT*4);
        for (U32 i = 0; i < WIDTH*HEIGHT; ++i) {
            if (_gr1[i] != 0xFF000000) {
                _gr[i] = _gr1[i];
            }
        }
        if (MouseBtn & 2) {
            GrDrawCircle(&gr1, MouseX, MouseY, 5, 0xFF00FF);
        }
        if (MouseBtn & 4) {
            Gr gr = (Gr) {
                .buffer = fb0.buffer,
                .width = WIDTH,
                .height = HEIGHT,
            };
            GrDrawCircle(&gr, MouseX, MouseY, 5, 0xFF00FF);
        }
        for (U32 i = 0; i < 4; ++i) {
            if (MouseX + MouseY * WIDTH + i < WIDTH*HEIGHT)
                _gr[MouseX + MouseY * WIDTH + i] = 0xFFFFFF - _gr[MouseX + MouseY * WIDTH + i];
            if (MouseX + (MouseY + i) * WIDTH < WIDTH*HEIGHT)
                _gr[MouseX + (MouseY + i) * WIDTH] = 0xFFFFFF - _gr[MouseX + (MouseY + i) * WIDTH];
        }
        for (U32 i = 1; i < 8; ++i) {
            if (MouseX + (MouseY + i) * WIDTH + i < WIDTH*HEIGHT)
                _gr[MouseX + (MouseY + i) * WIDTH + i] = 0xFFFFFF - _gr[MouseX + (MouseY + i) * WIDTH + i];
        }
        for (U32 i = 0; i < WIDTH * HEIGHT; ++i) {
            if (_gr[i] != _grp[i]) {
                fb0.buffer[i] = _gr[i];
                _grp[i] = _gr[i];
            }
        }
        U32 t = PITTime;
        while (PITTime - t < 1000 / 60 + (t - st));
    }
    for(;;);
}
