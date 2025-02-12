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


void KernelMain() {
    IDTInit();
    PICMap();
    PITInit();
    KBInit();
    VgaBlinkingSet(False);
    VgaCursorDisable();
    VgaCursorEnable();
    VgaCursorSet(0, 0);
    TTYClear();
    TTYUPrint(
        "\n"
        "$!EBosyOS control codes$!F: $*F$!0See *mezex.txt*$*0$!F\n"
        "$!EWrite something in keyboard$!F. $x9F");
    U32 inputbox = 320;
    Bool inputboxi = True;
    VgaCursorSetR(320);
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
                if (KBState.Key == F5) {
                    WordGen();
                }
                else
                    TTYUPrintC(KBState.Shift ? ToUpper(KBState.Key) : KBState.Key);
            }
            inputboxi = KBState.SC & 0x80;
        }
        inputbox = TTYCursor;
        MSleep(5);
    }
    CpuHalt();
}