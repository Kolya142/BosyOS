#include <drivers/input/mouse.h>
#include <drivers/misc/random.h>
#include <kernel/KWinDemo.h>
#include <kws/input.h>
#include <kws/win.h>
#include <stdarg.h>

static Bool active = True;

static U0 SpawnerUpd(Ptr this) {
    Win *win = this;
    WPrintF(win, 0, 0, "Exit");
    if (win->inp.mouse_left && MouseX >= win->x && MouseY >= win->y + 7 && MouseX <= win->w + 4*6 && MouseY <= win->y + 6 + 7) {
        active = False;
    }
    PielPixel(&win->canvas, RandomU() % win->w, RandomU() % win->h, RandomU()%16);
}

U0 KWinDemo() {
    Win spawner;
    spawner = WinMake(10, 10, 60, 100, "Spawner", 0);
    spawner.update = SpawnerUpd;
    WinSpawn(&spawner);
    while (active) {
        VRMClear(DBlue);
        WindowsUpdate();
        SleepM(100);
    }
}