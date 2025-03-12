#include <drivers/input/mouse.h>
#include <drivers/misc/random.h>
#include <kernel/KWinDemo.h>
#include <kws/input.h>
#include <kws/win.h>
#include <stdarg.h>

static U0 SpawnerUpd(Ptr this) {
    Win *win = this;
    PielPixel(&win->canvas, RandomU() % win->w, RandomU() % win->h, RandomU()%16);
}

static PielCanvas *canv;

static U0 RenderWG() {
    U32 cc = 0;
    U32 y = TTYGSY;
    for (Char c;PTermRead(&VTerm, 1, &c, 1);) {
        U32 x = cc * 6 + TTYGSX;
        PielBox(canv, x, y, 6, 6, Black);
        for (U32 i = 0; i < 5; ++i) {
            for (U32 j = 0; j < 5; ++j) {
                Bool bit = (TTYFont[c][i] >> (4-j)) & 1;
                PielPixel(canv, x+j, y+i, bit ? White : Black);
            }
        }
        ++cc;
    }
}

static U0 WPrintF(Win *win, U32 x, U32 y, String format, ...) {
    TTerm.render();
    TTYGSX = x;
    TTYGSY = y;
    canv = &win->canvas;
    Ptr rend = TTerm.render;
    TTerm.render = RenderWG;

    va_list args;
    va_start(args, format);
    VPrintF(format, args);
    va_end(args);

    TTerm.render();
    TTerm.render = rend;
}

static U0 TimeUpd(Ptr this) {
    Win *win = this;
    WPrintF(win, 0, 0, "%d:%d:%d", SystemTime.hour, SystemTime.minute, SystemTime.second);
}

U0 KWinDemo() {
    Win spawner;
    spawner = WinMake(10, 10, 60, 100, "Spawner");
    spawner.update = SpawnerUpd;
    Win time;
    time = WinMake(200, 10, 8*6, 6, "Time");
    time.update = TimeUpd;
    WinSpawn(&spawner);
    WinSpawn(&time);
    for(;;) {
        VRMClear(DBlue);
        WindowsUpdate();
        SleepM(100);
    }
}