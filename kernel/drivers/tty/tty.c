#include <lib/strings/String.h>
#include <lib/IO/TTY.h>
#include <drivers/tty.h>

U32 TTYCursor = 0;
U32 TTYGSX = 0;
U32 TTYGSY = 0;
PTerm *VTerm;
PTerm VTerms[4];
TTY TTerm;
U32 TTermID = 0;
Bool TTYCanonical = True;

U8 TTYBuffers[4][8192];
U32 TTYBuffersIndex[4];

U32 KBTimeout = 500;
U32 KBRate = 200;

static Bool lk[256] = {0};
static U32 bufferi = 0;
static U32 time = 0;
static U32 timea = 0;
static U8 buf[2048];

U0 TTYSwitch(U32 id) {
    if (id > sizeof(VTerms)/sizeof(VTerms[0])) {
        return;
    }

    TTYClear();
    TTYCursor = 0;
    TTerm.render();
    TTermID = id;
    VTerm = &VTerms[id];
    PTermWrite(VTerm, 1, TTYBuffers[id], TTYBuffersIndex[id]);
    MemSet(TTYBuffers[id], 0, 8192);
    TTYBuffersIndex[id] = 0;
}

U0 TTYInput() {
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
                        TTYUPrintC('\b');
                        TTerm.render();
                    }
                }
                else if (key == ASCIIPF1) TTYSwitch(0);
                else if (key == ASCIIPF2) TTYSwitch(1);
                else if (key == ASCIIPF3) TTYSwitch(2);
                else if (key == ASCIIPF4) TTYSwitch(3);
                else if (key == '\x1b') return;
                else if (key == '\r') {
                    TTYUPrintC('\n');
                    TTerm.render();
                    PTermWrite(VTerm, 0, buf, bufferi);
                    bufferi = 0;
                }
                else if (key < 0x80 || (key >= 0xB1 && key <= 0xD0)) {
                    if (KBState.Shift && !(key >= 0xB1 && key <= 0xD0)) {
                        key = UpperTo(key);
                    }
                    if (bufferi < sizeof(buf) - 1) {
                        buf[bufferi++] = key;
                        TTYUPrintC(key);
                        TTerm.render();
                    }
                }
            }
        }
        else if (!KBState.keys[key] && lk[key]) {
            lk[key] = False;
        }
    }
}