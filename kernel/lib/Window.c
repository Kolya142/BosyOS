#include <drivers/input/keyboard.h>
#include <drivers/input/mouse.h>
#include <drivers/time/pit.h>
#include <kernel/KHeap.h>
#include <lib/Window.h>
#include <lib/String.h>


U0 WindowDraw(Window *win, Gr *gr) {
    U32 main_color = win->red ? 0x800000 : 0x0080FF;
    for (U32 j = 1; j < win->height - 1; ++j) {
        GrPutChar(gr, 0xA3, (win->x) * 8, (j + win->y) * 8, main_color, 0xFFFFFF);
        GrPutChar(gr, 0xA3, (win->x + win->width - 1) * 8, (j + win->y) * 8, main_color, 0xFFFFFF);
    }
    {
        U32 s = 0;
        while (win->title[s]) ++s;
        U32 t = 0;
        for (U32 i = 1; i < win->width; ++i) {
            if ((PITTime % 4000) < 2000) {
                GrPutChar(gr, win->title[t], win->x * 8 + (i * 8 + (PITTime / 100)) % (win->width * 8 - 8), win->y * 8, 0xFFFFFF, main_color);
            }
            else {
                GrPutChar(gr, win->title[t], win->x * 8 + (i * 8 + (PITTime / 100)) % (win->width * 8 - 8), win->y * 8, main_color, 0xFFFFFF);
            }
            ++t;
            if (!win->title[t]) t = 0;
            GrPutChar(gr, 0xA4, (i + win->x) * 8, (win->y + win->height - 1) * 8, main_color, 0xFFFFFF);
        }
    }
    GrPutChar(gr, 0x83, (win->x) * 8, (win->y) * 8, main_color, 0xFFFFFF);
    GrPutChar(gr, 0x85, (win->x + win->width - 1) * 8, (win->y) * 8, main_color, 0xFFFFFF);
    GrPutChar(gr, 0xA5, (win->x) * 8, (win->y + win->height - 1) * 8, main_color, 0xFFFFFF);
    GrPutChar(gr, 0x86, (win->x + win->width - 1) * 8, (win->y + win->height - 1) * 8, main_color, 0xFFFFFF);

    Bool blinking = False;

    U32 fg = 0x000000;
    U32 bg = 0xFFFFFF;
    U32 sel = 0;
    U32 p = 0;
    for (U32 x = 0; p < (win->width - 2) * (win->height - 2); ++x) {
        Char c = win->text_buffer[x];
        if (sel) {
            if (sel == 1) {
                switch (c) {
                    case 'B': {
                        blinking = !blinking;
                        sel = 0;
                    } break;
                    case 'f': {
                        sel = 2;
                    } break;
                    case 'b': {
                        sel = 3;
                    } break;
                    case 'U': {
                        p -= win->width - 2;
                        sel = 0;
                    } break;
                    case 'D': {
                        p += win->width - 2;
                        sel = 0;
                    } break;
                    case 'L': {
                        --p;
                        sel = 0;
                    } break;
                    case 'M': {
                        sel = 4;
                    } break;
                    case 'R': {
                        ++p;
                        sel = 0;
                    } break;
                    case 'P': {
                        sel = 0;
                    } break;
                    case '!': {
                        blinking = False;
                        fg = 0x000000;
                        bg = 0xFFFFFF;
                        sel = 0;
                    } break;
                    case '$': {
                        U32 i = (win->x + 1) * 8 + (p % (win->width - 2)) * 8;
                        U32 j = (win->y + 1) * 8 + (p / (win->width - 2)) * 8;
                        if (blinking && (PITTime % 4000) < 2000) {
                            GrPutChar(gr, '$', i, j, bg, fg);
                        }
                        else {
                            GrPutChar(gr, '$', i, j, fg, bg);
                        }
                        ++p;
                        sel = 0;
                    } break;
                    default: {
                        U32 i = (win->x + 1) * 8 + (p % (win->width - 2)) * 8;
                        U32 j = (win->y + 1) * 8 + (p / (win->width - 2)) * 8;
                        if (blinking && (PITTime % 4000) < 2000) {
                            GrPutChar(gr, '$', i, j, bg, fg);
                        }
                        else {
                            GrPutChar(gr, '$', i, j, fg, bg);
                        }
                        ++p;
                        i = (win->x + 1) * 8 + (p % (win->width - 2)) * 8;
                        j = (win->y + 1) * 8 + (p / (win->width - 2)) * 8;
                        if (blinking && (PITTime % 4000) < 2000) {
                            GrPutChar(gr, c, i, j, bg, fg);
                        }
                        else {
                            GrPutChar(gr, c, i, j, fg, bg);
                        }
                        ++p;
                        sel = 0;
                    }
                }
            }
            else if (sel == 2) {
                switch (c) {
                    case 'r': {
                        fg ^= 0xFF0000;
                    } break;
                    case 'g': {
                        fg ^= 0x00FF00;
                    } break;
                    case 'b': {
                        fg ^= 0x0000FF;
                    } break;
                }
                sel = 0;
            }
            else if (sel == 3) {
                switch (c) {
                    case 'r': {
                        bg ^= 0xFF0000;
                    } break;
                    case 'g': {
                        bg ^= 0x00FF00;
                    } break;
                    case 'b': {
                        bg ^= 0x0000FF;
                    } break;
                }
                sel = 0;
            }
            else if (sel == 4) {
                switch (c) {
                    case 'X': {
                        p -= p % (win->width - 2);
                        p += (win->width - 2) / 2;
                    } break;
                }
                sel = 0;
            }
        }
        else if (c == '\n') {
            p += (win->width - 2) - (p % (win->width - 2));
        }
        else if (c == '$') {
            sel = 1;
        }
        else {
            U32 i = (win->x + 1) * 8 + (p % (win->width - 2)) * 8;
            U32 j = (win->y + 1) * 8 + (p / (win->width - 2)) * 8;
            if (blinking && (PITTime % 4000) < 2000) {
                GrPutChar(gr, c, i, j, bg, fg);
            }
            else {
                GrPutChar(gr, c, i, j, fg, bg);
            }
            ++p;
        }
    }
    if (win->draw) {
        win->draw(win);
    }
}
Window WindowInit(U0(*draw)(Window *win)) {
    return (Window) {
        .red = False,
        .title = "Term",
        .text_buffer = KMAlloc(4096),
        .x = 5,
        .y = 5,
        .z = 0,
        .width = 30,
        .height = 20,
        .flags = 1 | 2 | 4,
	.draw = draw
    };
}
static Bool lk[256];
U0 WindowsUpd(Gr *gr, Window **windows, U32 wincounts) {
    I32 zmin = I32_MAX;
    I32 zmax = I32_MIN;

    for (U32 i = 0; i < wincounts - 1; ++i) {
        for (U32 j = 0; j < wincounts - i - 1; ++j) {
            if (windows[j]->z > windows[j+1]->z) {
                Window *tmp = windows[j];
                windows[j] = windows[j+1];
                windows[j+1] = tmp;
            }
        }
    }

    zmin = windows[0]->z;
    zmax = windows[wincounts - 1]->z;

    for (U32 i = 0; i < wincounts; ++i) {
        WindowDraw(windows[i], gr);
    }
    Bool grapping = False;
    for (I32 i = wincounts - 1; i >= 0; --i) {
        if (!grapping && (windows[i]->flags & 1)) {
            if (windows[i]->isgrapping) {
                if (MouseBtn & 1) {
                    windows[i]->x = (MouseX - windows[i]->gx) / 8;
                    windows[i]->y = (MouseY - windows[i]->gy) / 8;
                    grapping = True;
                }
                else {
                    windows[i]->isgrapping = False;
                }
            }
            if ((MouseX >= windows[i]->x * 8) && (MouseX <= (windows[i]->x + windows[i]->width) * 8) && (MouseY >= windows[i]->y * 8) && (MouseY <= windows[i]->y * 8 + windows[i]->height * 8)) {
                if ((MouseY <= windows[i]->y * 8 + 8)) {
                    windows[i]->isgrapping = True;
                    windows[i]->gx = MouseX - windows[i]->x * 8;
                    windows[i]->gy = MouseY - windows[i]->y * 8;
                }
                if (windows[i]->flags & 2) {
                    for (U16 key = 0; key < 256; ++key) {
                        if (KBState.Keys[key]) {
                            if (!lk[key]) {
                                lk[key] = True;
                                if (key == '\b') {
                                    U32 s = StrLen(windows[i]->text_buffer);
                                    if (!(windows[i]->text_buffer[s-2] == '$' && windows[i]->text_buffer[s-1] == 'P')) {
                                        WindowPrint(windows[i], "\b");
                                    }
                                }
                                else if (!(key & 0x80)) {
                                    Char k = KBState.Shift ? Upper(key) : key;
                                    WindowPrint(windows[i], (Char[]) {k, 0});
                                }
                            }
                        }
                        else {
                            lk[key] = False;
                        }
                    }
                }
                windows[i]->z = zmax + 1;
                for (U32 j = 0; j < wincounts; ++j) {
                    --windows[j]->z;
                }
                grapping = True;
            }
        }
    }
}
U0 WindowPrint(Window *win, String text) {
    U32 start = StrLen(win->text_buffer);
    U32 size = StrLen(text);
    U32 pos = 0;
    for (U32 i = 0; i < size; ++i) {
        switch (text[i]) {
            case '\b': {
                if (start + pos) {
                    --pos;
                    win->text_buffer[start + pos] = 0;
                }
            } break;
            case '\r': {
                win->text_buffer[start + pos] = '\n';
                ++pos;
            } break;
            default: {
                win->text_buffer[start + pos] = text[i];
                ++pos;
            } break;
        }
    }
}
U0 WindowPrintNum(Window *win, U32 num) {
    if (!num) {
        WindowPrint(win, "0");
        return;
    }
    Char buf[11] = {0};
    U32 size = 0;
    for (U32 i = num; i > 0; i /= 10) {
        ++size;
    }
    U32 bufi = size - 1;
    for (U32 i = num; i > 0; i /= 10) {
        buf[bufi--] = '0' + (i % 10);
    }
    MemCpy(win->text_buffer + StrLen(win->text_buffer), buf, size);
}
String WindowGetLine(Window *win) {
    U32 len = StrLen(win->text_buffer);

    if (len < 2) {
        return NULL;
    }

    for (U32 i = len - 1; i >= 1; --i) {
        if (win->text_buffer[i - 1] == '$' && win->text_buffer[i] == 'P') {
            U32 s = len - i + 1;
            String line = KMAlloc(s + 1);
            MemCpy(line, &win->text_buffer[i - 1], s);
            line[s] = 0;
            return line;
        }
    }

    return NULL;
}
