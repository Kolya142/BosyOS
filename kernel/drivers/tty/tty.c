#include <drivers/input/keyboard.h>
#include <drivers/time/pit.h>
#include <lib/strings/String.h>
#include <drivers/tty.h>
#include <kernel/KTasks.h>

List PTYs;
List TTYs;
U32 TTYCurrent;

U0 TTYSwitch(U32 id) {
    ++id;
    if (id >= TTYs.count) {
        return;
    }
    TTYCurrent = id;
    TTY *t = &((TTY*)TTYs.arr)[id];
    PTYWrite(((TTY*)TTYs.arr)[TTYCurrent].pty, 1, (U8[]) {ASCIIPCtrl}, 1);
    TTYFlush(TTYCurrent);
    PTYWrite(((TTY*)TTYs.arr)[TTYCurrent].pty, 1, t->last, t->last_index);
    TTYFlush(TTYCurrent);
}
U0 TTYInput() {
    static Bool lk[256] = {0};
    static U32 bufferi = 0;
    static U32 time = 0;
    static U32 timea = 0;
    static U8 buf[2048] = {0};

    for (U16 key = 0; key < 256; ++key) {
        if (KBState.keys[key]) {
            if (!lk[key] || ((PITTime - time >= KBTimeout) && (PITTime - timea >= KBRate))) {
                if (!lk[key]) {
                    time = PITTime;
                }
                else {
                    timea = PITTime;
                }
                lk[key] = True;
                if (key == '\b') {
                    if (bufferi) {
                        buf[bufferi--] = 0;
                        TTYWrite(TTYCurrent, 1, "\b", 1);
                        TTYFlush(TTYCurrent);
                    }
                }
                else if (key == ASCIIPF1) TTYSwitch(0);
                else if (key == ASCIIPF2) TTYSwitch(1);
                else if (key == ASCIIPF3) TTYSwitch(2);
                else if (key == ASCIIPF4) TTYSwitch(3);
                else if (key == '\x1b') return;
                else if (key == 'c' && KBState.Ctrl) {
                    // TaskSigSend(TaskTail, SIGTERM);
                }
                else if (key == 'l' && KBState.Ctrl && TaskTail) {
                    TTYWrite(TaskTail->ttyid, 1, "\x80", 1);
                }
                else if (key == 'u' && KBState.Ctrl && TaskTail) {
                    for (; bufferi; --bufferi) {
                        TTYWrite(TaskTail->ttyid, 1, "\b", 1);
                    }
                }
                else if (key == '\r') {
                    TTYWrite(TTYCurrent, 1, "\n", 1);
                    TTYFlush(TTYCurrent);
                    TTYWrite(TTYCurrent, 0, buf, bufferi);
                    bufferi = 0;
                }
                else if (key < 0x80 || (key >= 0xB1 && key <= 0xD0)) {
                    if (KBState.Shift && !(key >= 0xB1 && key <= 0xD0)) {
                        key = UpperTo(key);
                    }
                    if (bufferi < sizeof(buf) - 1) {
                        buf[bufferi++] = key;
                        Char k = key;
                        TTYWrite(TTYCurrent, 1, &k, 1);
                        TTYFlush(TTYCurrent);
                    }
                }
            }
        }
        else if (!KBState.keys[key] && lk[key]) {
            lk[key] = False;
        }
    }
}

U0 TTYInit() {
    PTYs = ListInit(sizeof(PTY));
    TTYs = ListInit(sizeof(TTY));
}
U32 PTYNew(U32 size, U32 width, U32 height) {
    PTY pty;
    PTYMake(&pty, size, width, height);
    ListAppend(&PTYs, &pty);
    return PTYs.count - 1;
}
U32 TTYNew(U0(*flush)(TTY *this, U32 ttyid), U32 pty) {
    if (pty >= PTYs.count) return -1;
    TTY tty;
    tty.pty = &((PTY*)PTYs.arr)[pty];
    tty.flush = flush;
    tty.last_index = 0;
    MemSet(tty.data, 0xFF, sizeof(tty.data));
    ListAppend(&TTYs, &tty);
    return TTYs.count - 1;
}
U32 TTYWrite(U32 tty, U32 pd, String content, U32 count) {
    if (tty >= TTYs.count) return 0;
    TTY *t = &((TTY*)TTYs.arr)[tty];
    for (U32 i = 0; i < count; ++i) {
        if (t->last_index < 2048) {
            t->last[t->last_index++] = content[i];
        } else {
            MemCpy(t->last, t->last + 1, 2047);
            t->last[2047] = content[i];
        }
    }
    return PTYWrite(t->pty, pd, content, count);
}
U32 TTYRead(U32 tty, U32 pd, String content, U32 count) {
    if (tty >= TTYs.count) return 0;
    return PTYRead(((TTY*)TTYs.arr)[tty].pty, pd, content, count);
}
U0 TTYFlush(U32 tty) {
    if (tty >= TTYs.count) return;
    TTY *t = &((TTY*)TTYs.arr)[tty];
    t->flush(t, tty);
}