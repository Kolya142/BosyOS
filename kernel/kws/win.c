#include <lib/String.h>
#include <kws/win.h>

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
    WinPrint(win->x, win->y+1, win->title);

    for (U32 i = 0; i < win->w; ++i) {
        for (U32 j = 0; j < win->h; ++j) {
            VRMPSet(i+win->x, j+win->y+8, win->canvas.arr[i+j*win->w]);
        }
    }
    VRMState = True;
}