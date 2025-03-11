#include <lib/IO/KeyboardLib.h>
#include <drivers/time/pit.h>
#include <lib/strings/String.h>
#include <lib/memory/MemLib.h>
#include <lib/IO/TTY.h>

U32 KBTimeout = 500;
U32 KBRate = 200;

U0 KBRead(String buf, U32 count) {
    Bool lk[256];
    MemCpy(lk, KBState.keys, 256);
    U32 bufferi = 0;
    U32 time = PITTime;
    U32 timea = PITTime;
    for (;;) {
        for (U8 key = 0; key < 256; ++key) {
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
                            TTYUPrintC(KBState.Key);
                            TTerm.render();
                        }
                    }
                    else if (key == '\x1b') return;
                    else if (key == '\r') {
                        TTYUPrintC('\n');
                        TTerm.render();
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
}