#include "windows.h"
#define CHARBORDERVER 0xBA // ║
#define CHARBORDERHOZ 0xCD // ═
#define CHARBORDERLT  0xC9 // ╔
#define CHARBORDERLB  0xC8 // ╚
#define CHARBORDERRT  0xBB // ╗
#define CHARBORDERRB  0xBC // ╝

WindowData *WindowNew(WindowHandler handler)
{
    WindowData *wdata = HeapAlloc(sizeof(WindowData));
    wdata->handler = handler;
    wdata->w = 10;
    wdata->h = 3;
    wdata->x = 5;
    wdata->y = 5;
    wdata->title_shift = 0;
    wdata->title_len = 10;
    MemCpy(wdata->title, "New Window", sizeof("New Window"));
    return wdata;
}
void WindowUpdate(WindowData *window)
{
    window->title_shift = window->title_shift+1;
    /*
    ╔═══╗
    ║WOW║
    ╚═══╝
    */
    TTYPrintCharPos(CHARBORDERLT, window->x, window->y);
    TTYPrintCharPos(CHARBORDERRT, window->x+window->w, window->y);
    TTYPrintCharPos(CHARBORDERLB, window->x, window->y+window->h);
    TTYPrintCharPos(CHARBORDERRB, window->x+window->w, window->y+window->h);
    for (int i = window->x+1; i < window->x+window->w; i++) {
        TTYPrintCharPos(window->title[(window->title_shift/10+i-window->x)%window->title_len], i, window->y);
        TTYPrintCharPos(CHARBORDERHOZ, i, window->y+window->h);
    }
    for (int i = window->y+1; i < window->y+window->h; i++) {
        TTYPrintCharPos(CHARBORDERVER, window->x, i);
        TTYPrintCharPos(CHARBORDERVER, window->x+window->w, i);
    }
    window->handler(window);
}