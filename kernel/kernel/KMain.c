#include <drivers/controllers/ps2.h>
#include <drivers/input/keyboard.h>
#include <drivers/input/mouse.h>
#include <lang/Interpretter.h>
#include <drivers/time/rtc.h>
#include <drivers/time/pit.h>
#include <arch/i386/gdt.h>
#include <kernel/KHeap.h>
#include <misc/screen.h>
#include <lib/Window.h>
#include <lib/String.h>
#include <lib/MemLib.h>
#include <lib/Types.h>
#include <multiboot.h>
Gr gr0;
Gr gr1;
static U0 test_window_upd(Window *win) {
    for (U32 i = 8; i < win->width * 8 - 8; ++i) {
	if (i+win->x*8 > WIDTH) continue;
	for (U32 j = 8; j < win->height * 8 - 8; ++j) {
	    if (j+win->y*8 > HEIGHT) continue;
	    GrDrawPixel(&gr0, i+win->x*8, (win->height * 8)-j+win->y*8, i&j ? 0xFFFFFF : 0x000000);
	}
    }
}

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
    Gr gr = (Gr) {
	.buffer = _gr,
	.width = WIDTH,
	.height = HEIGHT
    };

    U32 *_gr0 = KMAlloc(WIDTH * HEIGHT * 4);
    gr0 = (Gr) {
        .buffer = _gr0,
        .width = WIDTH,
        .height = HEIGHT
    };
    U32 *_gr1 = KMAlloc(WIDTH * HEIGHT * 4);
    gr1 = (Gr) {
        .buffer = _gr1,
        .width = WIDTH,
        .height = HEIGHT
    };
    Window winpool[] = {
        WindowInit(NULL),
        WindowInit(NULL),
        WindowInit(NULL),
	WindowInit(test_window_upd),
    };
    Window *winlist[] = {
        &winpool[0],
        &winpool[1],
        &winpool[2],
	&winpool[3],
    };
    winpool[0].x = 0;
    winpool[0].y = 0;
    winpool[0].z = -4;
    winpool[0].width = gr0.width / 8;
    winpool[0].height = 10;
    winpool[0].title = "Info";
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
    for (U32 i = 0; i < WIDTH*HEIGHT; ++i) {
        _gr1[i] = 0xFF000000;
    }
    U32 mouse_px = 0;
    U32 mouse_py = 0;
    for (;;) {
        RTCUpdate();
        String line = WindowGetLine(&winpool[1]);
        MemSet(winpool[2].text_buffer, 0, 4096);
        MemCpy(winpool[2].text_buffer, line, StrLen(line));
        KMFree(line);
        MemSet(winpool[0].text_buffer, 0, 4096);
        WindowPrint(&winpool[0], "BosyOS 1.0.1\n");
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
        WindowPrint(&winpool[0], "\n$BTime from UTC$B: ");
	WindowPrintNum(&winpool[0], SystemTime.hour);
        WindowPrint(&winpool[0], ".");
	WindowPrintNum(&winpool[0], SystemTime.minute);
        WindowPrint(&winpool[0], ".");
	WindowPrintNum(&winpool[0], SystemTime.second);
	if (KBState.Ctrl && KBState.Shift && KBState.Keys['g']) {
	    grid = False;
	}
	else if (KBState.Ctrl && KBState.Keys['g']) {
	    grid = True;
	}
	else if (KBState.Ctrl && KBState.Keys['1']) {
	    GrDrawRect(&gr1, mouse_px, mouse_py, MouseX - mouse_px, MouseY - mouse_py, 0xFF00FF);
	}
	else if (KBState.Ctrl && KBState.Keys['2']) {
	    GrDrawLine(&gr1, mouse_px, mouse_py, MouseX, MouseY, 0xFF00FF);
	}
	else if (KBState.Ctrl && KBState.Keys['3']) {
	    U32 r = (I32)(MouseX - mouse_px) < 0 ? -(MouseX - mouse_px) : (MouseX - mouse_px);
	    GrDrawCircle(&gr1, MouseX - (r >> 1), MouseY - (r >> 1), r, 0xFF00FF);
	}
	// WindowPrint(&winpool[0], KHeapSanityCheck() ? "\n$frHeap corrupted$fr\n" : "\n$fgHeap Valid$fg\n"); U32 st = PITTime;
        for (U32 i = 0; i < gr0.width * gr0.height; ++i) {
            gr0.buffer[i] = 0xFFFFFF;
        }

        WindowsUpd(&gr0, winlist, 4);

        MemCpy(_gr, _gr0, WIDTH*HEIGHT*4);
        for (U32 i = 0; i < WIDTH*HEIGHT; ++i) {
            if (_gr1[i] != 0xFF000000) {
                _gr[i] = _gr1[i];
            }
        }
        if (MouseBtn & 2) {
	    mouse_px = MouseX;
	    mouse_py = MouseY;
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
	if (grid) {
	    for (U32 i = 0; i < WIDTH; i += 8) {
		for (U32 j = 0; j < HEIGHT; ++j) {
		    _gr[i+j*WIDTH] = 0x000000;
		}
	    }
	    for (U32 j = 0; j < HEIGHT; j += 8) {
		for (U32 i = 0; i < WIDTH; ++i) {
		    _gr[i+j*WIDTH] = 0x000000;
		}
	    }
	}
        for (U32 i = 0; i < WIDTH * HEIGHT; ++i) {
            if (_gr[i] != _grp[i]) {
                fb0.buffer[i] = _gr[i];
                _grp[i] = _gr[i];
            }
	}
        // U32 t = PITTime;
        // while (PITTime - t < 1000 / 60);
    }
    for(;;);
}
