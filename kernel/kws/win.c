#include <lib/String.h>
#include <kws/win.h>

U0 WinPrint(U32 x, U32 y, String text) {
    U32 c = TTYCursor;
    U32 wx = TTYCursorX;
    U32 wy = TTYCursorY;
    TTYCursor = 0;
    TTYCursorX = x;
    TTYCursorY = y;
    TTYUPrint(text);
    TTYCursor = c;
    TTYCursorX = wx;
    TTYCursorY = wy;
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
    WinPrint(win->x, win->y, win->title);

    for (U32 i = 0; i < win->w; ++i) {
        for (U32 j = 0; j < win->h; ++j) {
            VRMPSet(i+win->x, j+win->y+8, win->canvas.arr[i+j*win->w]);
        }
    }
    VRMState = True;
}