#include <lib/KeyboardLib.h>
#include <lib/String.h>
#include <lib/MemLib.h>
#include <lib/TTY.h>

U0 KBRead(String buf, U32 count) {
    Bool lk[128];
    MemCpy(lk, KBState.keys, 128);
    U32 bufferi = 0;
    for (;;) {
        for (U8 key = 0; key < 128; ++key) {
            if (KBState.keys[key] && !lk[key]) {
                lk[key] = True;
                if (!KBState.Key) continue;
                if (KBState.Key < 0x80) {
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
                    else {
                        if (KBState.Shift) key = ToUpper(key);
                        if (bufferi < count - 1) {
                            buf[bufferi++] = key;
                            TTYUPrintC(key);
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