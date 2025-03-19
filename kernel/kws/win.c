#include <drivers/input/mouse.h>
#include <lib/strings/String.h>
#include <kws/win.h>

Win windows[64];

static PielCanvas *canv;

static U0 RenderWG() {
    // U32 cc = 0; // FIXME
    // U32 y = TTYGSY;
    // for (Char c;PTermRead(VTerm, 1, &c, 1);) {
    //     U32 x = cc * 6 + TTYGSX;
    //     PielBox(canv, x, y, 6, 6, Black);
    //     for (U32 i = 0; i < 5; ++i) {
    //         for (U32 j = 0; j < 5; ++j) {
    //             Bool bit = (TTYFont[c][i] >> (4-j)) & 1;
    //             PielPixel(canv, x+j, y+i, bit ? White : Black);
    //         }
    //     }
    //     ++cc;
    // }
}

U0 WPrintF(Win *win, U32 x, U32 y, String format, ...) {
    // TTerm.render(); // FIXME
    // TTYGSX = x;
    // TTYGSY = y;
    // canv = &win->canvas;
    // Ptr rend = TTerm.render;
    // TTerm.render = RenderWG;

    // va_list args;
    // va_start(args, format);
    // VPrintF(format, args);
    // va_end(args);

    // TTerm.render();
    // TTerm.render = rend;
}

U32 WinSpawn(Win *win) {
    for (U32 i = 0; i < 64; ++i) {
        if (!(windows[i].w)) {
            MemCpy(&windows[i], win, sizeof(Win));
            return i;
        }
    }
}
U0 WindowsUpdate() {
    for (U32 i = 0; i < 64; ++i) {
        if (windows[i].w) {
            Win *win = &windows[i];
            if (!(win->flags & WIN_UNMOVEBLE) && (MouseBtn & 1) && win->x <= MouseX && MouseX <= win->x+win->w-7 && win->y <= MouseY && MouseY <= win->y+7 && MouseX > win->w / 2 && MouseY > 3) {
                win->x = MouseX - win->w / 2;
                win->y = MouseY - 3;
            }
            else if (!(win->flags & WIN_UNCLOSABLE) && win->inp.mouse_left && win->x+win->w-7 <= MouseX && MouseX <= win->x+win->w && win->y <= MouseY && MouseY <= win->y+7) {
                win->w = 0;
                continue;
            }
            KWSUpdate(&win->inp);
            if (win->update) {
                win->update(win);
            }
            WinDraw(win);
        }
    }
}

U0 WinPrint(U32 x, U32 y, String text) {
    // TTYGSX = x; // FIXME
    // TTYGSY = y;
    // Ptr render = TTerm.render;
    // TTerm.render = TTYRenderGS;
    // TTYUPrint(text);
    // TTerm.render = render;
}
Win WinMake(U32 x, U32 y, U32 w, U32 h, String title, U16 flags) {
    Win win;
    win.x = x;
    win.y = y;
    win.w = w;
    win.h = h;
    win.flags = flags;
    MemSet(win.title, 0, sizeof(win.title));
    StrCpy(win.title, title);
    
    win.canvas.arr = MAlloc(w * h);
    MemSet(win.canvas.arr, 0, w * h);
    win.canvas.height = h - 8;
    win.canvas.rwidth = w;
    win.canvas.width = w;
    return win;
}
extern Bool VRMState;
U0 WinDraw(Win *win) {
    VRMState = False;
    VRMDrawRect(vec2(win->x, win->y), vec2(win->x+win->w, win->y+7), White);
    VRMDrawRect(vec2(win->x, win->y+7), vec2(win->x+win->w, win->y+8), Gray);
    VRMDrawRect(vec2(win->x - 1, win->y), vec2(win->x, win->y + win->h + 1 + 7), White);
    VRMDrawRect(vec2(win->x + win->w, win->y), vec2(win->x + win->w + 1, win->y + win->h + 1 + 7), Gray);
    VRMDrawRect(vec2(win->x - 1, win->y + win->h + 1 + 7), vec2(win->x + win->w + 1, win->y + win->h + 1 + 1 + 7), Gray);
    if (!(win->flags & WIN_UNCLOSABLE)) {
        VRMDrawRect(vec2(win->x+win->w-7, win->y+1), vec2(win->x+win->w-1, win->y+7), Red);
    }
    WinPrint(win->x, win->y+1, win->title);

    for (U32 i = 0; i < win->w; ++i) {
        for (U32 j = 0; j < win->h; ++j) {
            VRMPSet(i+win->x, j+win->y+8, win->canvas.arr[i+j*win->w]);
        }
    }
    VRMState = True;
}