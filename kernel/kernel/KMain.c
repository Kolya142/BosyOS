#include <arch/cpu.h>
#include <arch/sys.h>
#include <arch/pic.h>
#include <drivers/keyboard.h>
#include <drivers/vga.h>
#include <libs/tty.h>
#include <drivers/pit.h>
#include <misc/time.h>
#include <misc/string.h>
#include <misc/meml.h>
#include <misc/wordgen.h>
#include <misc/cp517.h>
#include <misc/bf.h>

void KernelMain() {
    IDTInit();
    PICMap();
    PITInit();
    KBInit();
    VgaBlinkingSet(False);
    VgaCursorDisable();
    VgaCursorEnable();
    VgaCursorSet(0, 0);
    // MemSet(cp517, 0xff, sizeof(cp517));
    // VgaFontLoad(cp517);
    TTYClear();
    TTYUPrint(
        "\n"
        "$!EBosyOS control codes$!F: $*F$!0See *mezex.txt*$*0$!F\n"
        "$!EWrite bf code in keyboard$!F. $x9F");
    U32 inputbox = 320;
    U32 inputboxp = 0;
    Char inputboxd[500] = {0};
    Bool inputboxi = True;
    I32 stack[30] = {0};
    U8 mem[5000] = {0};
    I32 stacki = 0;
    for (;;) {
        TTYCursor = 0;
        {
            const String hex = "0123456789ABCDEF";
            U32i *t = (U32i*)&PITTime;
            TTYRawPrint(hex[t->u8[3] >> 4], Blue, White);
            TTYRawPrint(hex[t->u8[3] & 15], Blue, White);
            TTYRawPrint(hex[t->u8[2] >> 4], Blue, White);
            TTYRawPrint(hex[t->u8[2] & 15], Blue, White);
            TTYRawPrint(hex[t->u8[1] >> 4], Blue, White);
            TTYRawPrint(hex[t->u8[1] & 15], Blue, White);
            TTYRawPrint(hex[t->u8[0] >> 4], Blue, White);
            TTYRawPrint(hex[t->u8[0] & 15], Blue, White);
        }
        TTYCursor = inputbox;
        if ((KBState.Key != Lshift) && (KBState.Key != Rshift)) {
            if (!(KBState.SC & 0x80) && inputboxi) {
                if (KBState.Key == '\r') {
                    TTYCursor += 80;
                    TTYCursor -= TTYCursor % 80;
                    TTYPrint(inputboxd);
                    BFInterpret(inputboxd, mem, Null, stack, &stacki, 0, Null);
                    MemSet(inputboxd, 0, 500);
                    inputbox += 80;
                    inputbox -= inputbox % 80;
                    inputboxp = 0;
                }
                if (KBState.Key == '\b') {
                    inputboxp++;
                    inputboxd[inputboxp] = 0;
                    TTYPrintC('\b');
                }
                else {
                    Char k = KBState.Shift ? ToUpper(KBState.Key) : KBState.Key;
                    inputboxd[inputboxp] = k;
                    inputboxp++;
                    TTYPrintC(k);
                }
            }
            VgaCursorSetR(TTYCursor);
            inputboxi = KBState.SC & 0x80;
        }
        inputbox = TTYCursor;
        MSleep(5);
    }
    PowerOff();
}