#include <drivers/keyboard.h>
#include <user/syscall.h>
#include <misc/wordgen.h>
#include <arch/getreg.h>
#include <drivers/vga.h>
#include <drivers/pit.h>
#include <misc/string.h>
#include <misc/cp517.h>
#include <misc/time.h>
#include <misc/meml.h>
#include <user/bsf.h>
#include <arch/cpu.h>
#include <arch/sys.h>
#include <arch/pic.h>
#include <libs/tty.h>
#include <misc/bf.h>

U0 SysCallTest();

void KernelMain() {
    if (*(Byte*)(0x7C00 + 510) == 0x55) {
        for (U32 i = 0x7C00; i < 0x8000; i++) { // Clear bootloader
            *(Byte*)i = 0;
        }
    }
    IDTInit();
    PICMap();
    PITInit();
    KBInit();
    VgaBlinkingSet(False);
    VgaCursorDisable();
    VgaCursorEnable();
    VgaCursorSet(0, 0);
    SysCallSet(SysCallTest, 1);
    SysCallInit();
    // MemSet(cp517, 0xff, sizeof(cp517));
    // VgaFontLoad(cp517);
    TTYClear();
    TTYUPrint(
        "\n"
        "$!EBosyOS control codes$!F: $*F$!0See *mezex.txt*$*0$!F\n"
        "$!ERun a test application$!F.\n");
    {
        U8 appdata[] = "f\xb8\x01\0\xbe\x0cP\0\0\xcdx\xc3$!AHello, world!$!F\n\0";
        BsfHeader head = {
            .CodeS = 32,
            .Magic = 0x424f5359,
        };
        BsfApp app = {
            .header = head,
            .data = appdata,
        };
        BsfExec(&app);
    }
    TTYUPrint(
        "$!Aapplication ended$!F.\n");
    // U32 inputbox = 320;
    // U32 inputboxp = 0;
    // Char inputboxd[500] = {0};
    // Bool inputboxi = True;
    // I32 stack[30] = {0};
    // U8 mem[5000] = {0};
    // I32 stacki = 0;
    // for (;;) {
    //     TTYCursor = 0;
    //     {
    //         const String hex = "0123456789ABCDEF";
    //         U32i *t = (U32i*)&PITTime;
    //         TTYRawPrint(hex[t->u8[3] >> 4], Blue, White);
    //         TTYRawPrint(hex[t->u8[3] & 15], Blue, White);
    //         TTYRawPrint(hex[t->u8[2] >> 4], Blue, White);
    //         TTYRawPrint(hex[t->u8[2] & 15], Blue, White);
    //         TTYRawPrint(hex[t->u8[1] >> 4], Blue, White);
    //         TTYRawPrint(hex[t->u8[1] & 15], Blue, White);
    //         TTYRawPrint(hex[t->u8[0] >> 4], Blue, White);
    //         TTYRawPrint(hex[t->u8[0] & 15], Blue, White);
    //     }
    //     TTYCursor = inputbox;
    //     if ((KBState.Key != Lshift) && (KBState.Key != Rshift)) {
    //         if (!(KBState.SC & 0x80) && inputboxi) {
    //             if (KBState.Key == F5) {
    //                 WordGen();
    //             }
    //             else if (KBState.Key == '\r') {
    //                 TTYCursor += 80;
    //                 TTYCursor -= TTYCursor % 80;
    //                 TTYPrint(inputboxd);
    //                 if (!StrCmp(inputboxd, "cls")) {
    //                     MemSet(inputboxd, 0, 500);
    //                     inputbox = 320;
    //                     MemSet(vga+320*2,0, 4000-320*2);
    //                     inputboxp = 0;
    //                     continue;
    //                 }
    //                 BFInterpret(inputboxd, mem, Null, stack, &stacki, 0, Null);
    //                 MemSet(inputboxd, 0, 500);
    //                 inputbox += 80;
    //                 inputbox -= inputbox % 80;
    //                 inputboxp = 0;
    //             }
    //             if (KBState.Key == '\b') {
    //                 inputboxp++;
    //                 inputboxd[inputboxp] = 0;
    //                 TTYPrintC('\b');
    //             }
    //             else {
    //                 Char k = KBState.Shift ? ToUpper(KBState.Key) : KBState.Key;
    //                 inputboxd[inputboxp] = k;
    //                 inputboxp++;
    //                 TTYPrintC(k);
    //             }
    //         }
    //         VgaCursorSetR(TTYCursor);
    //         inputboxi = KBState.SC & 0x80;
    //     }
    //     inputbox = TTYCursor;
    //     MSleep(5);
    // }
    PowerOff();
}

U0 SysCallTest() {
    U32 addr = RegGet(REG_ESI);
    const String hex = "0123456789ABCDEF";
    U32i *t = (U32i*)&addr;
    TTYRawPrint(hex[t->u8[3] >> 4], Blue, White);
    TTYRawPrint(hex[t->u8[3] & 15], Blue, White);
    TTYRawPrint(hex[t->u8[2] >> 4], Blue, White);
    TTYRawPrint(hex[t->u8[2] & 15], Blue, White);
    TTYRawPrint(hex[t->u8[1] >> 4], Blue, White);
    TTYRawPrint(hex[t->u8[1] & 15], Blue, White);
    TTYRawPrint(hex[t->u8[0] >> 4], Blue, White);
    TTYRawPrint(hex[t->u8[0] & 15], Blue, White);
    TTYUPrint((Char*)addr);
}