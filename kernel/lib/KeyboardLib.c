#include <lib/KeyboardLib.h>
#include <drivers/pit.h>
#include <lib/String.h>
#include <lib/MemLib.h>
#include <lib/TTY.h>

U32 KBTimeout = 500;
U32 KBRate = 10;

U0 KBRead(String buf, U32 count) {
    Bool lk[256];
    MemCpy(lk, KBState.keys, 256);
    U32 bufferi = 0;
    U32 time = PITTime;
    U32 timea = PITTime;
    for (;;) {
        for (U8 key = 0; key < 256; ++key) {
            if (lk[key]) {
                if ((PITTime - time >= KBTimeout) && (PITTime - timea >= KBRate)) {
                    timea = PITTime;
                }
                else {
                    continue;
                }
            }
            if (KBState.keys[key]) {
                lk[key] = True;
                if (!KBState.Key) {
                    continue;
                }
                else {
                    time = PITTime;
                }
                if (key == '\b') {
                    if (bufferi) {
                        buf[bufferi--] = 0;
                        TTYUPrintC(KBState.Key);
                    }
                }
                else if (key == '\x1b') return;
                else if (key == '\r') {
                    TTYUPrintC('\n');
                    buf[bufferi] = 0;
                    // TTYUPrint(buffer);
                    if (bufferi)
                        return;
                }
                else if (key < 0x80 || (key >= 0xB1 && key <= 0xD0)) {
                    if (KBState.Shift && !(key >= 0xB1 && key <= 0xD0)) key = UpperTo(key);
                    if (bufferi < count - 1) {
                        buf[bufferi++] = key;
                        TTYUPrintC(key);
                    }
                }
            }
            else if (!KBState.keys[key] && lk[key]) {
                lk[key] = False;
            }
        }
    }
}