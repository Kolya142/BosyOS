#include <drivers/input/mouse.h>
#include <lib/strings/String.h>
#include <kws/win.h>

Win windows[64];
static PielCanvas *print_canv;

static U0 WinPielPrint() {

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
            if ((MouseBtn & 1) && win->x <= MouseX && MouseX <= win->x+win->w-7 && win->y <= MouseY && MouseY <= win->y+7 && MouseX > win->w / 2 && MouseY > 3) {
                win->x = MouseX - win->w / 2;
                win->y = MouseY - 3;
            }
            else if (win->inp.mouse_left && win->x+win->w-7 <= MouseX && MouseX <= win->x+win->w && win->y <= MouseY && MouseY <= win->y+7) {
                win->w = 0;
            }
            KWSUpdate(&win->inp);
            win->update(win);
            WinDraw(win);
        }
    }
}

U0 WinPrint(U32 x, U32 y, String text) {
    TTYGSX = x;
    TTYGSY = y;
    Ptr render = TTerm.render;
    TTerm.render = TTYRenderGS;
    TTYUPrint(text);
    TTerm.render = render;
}
Win WinMake(U32 x, U32 y, U32 w, U32 h, String title) {
    Win win;
    win.x = x;
    win.y = y;
    win.w = w;
    win.h = h;
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
    VRMDrawRect(vec2(win->x+win->w-7, win->y+1), vec2(win->x+win->w-1, win->y+7), Red);
    WinPrint(win->x, win->y+1, win->title);

    for (U32 i = 0; i < win->w; ++i) {
        for (U32 j = 0; j < win->h; ++j) {
            VRMPSet(i+win->x, j+win->y+8, win->canvas.arr[i+j*win->w]);
        }
    }
    VRMState = True;
}