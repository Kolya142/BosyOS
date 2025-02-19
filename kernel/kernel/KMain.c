#include <drivers/keyboard.h>
#include <kernel/KDogWatch.h>
#include <kernel/KPanic.h>
#include <user/syscall.h>
#include <misc/wordgen.h>
#include <arch/getreg.h>
#include <drivers/vga.h>
#include <drivers/pit.h>
#include <misc/string.h>
#include <misc/cp517.h>
#include <misc/time.h>
#include <arch/beep.h>
#include <libs/bosz.h>
#include <misc/meml.h>
#include <user/user.h>
#include <user/bsf.h>
#include <arch/cpu.h>
#include <arch/gdt.h>
#include <arch/sys.h>
#include <arch/pic.h>
#include <libs/tty.h>

U0 SysCallTest();
U0 programtest();

__attribute__((naked)) U0 KernelMain() {
    if (*(Byte*)(0x7C00 + 510) == 0x55) {
        for (U32 i = 0x7C00; i < 0x8000; i++) { // Clear bootloader
            *(Byte*)i = 0;
        }
    }
    TTYClear();
    KDogWatchInit();
    GDTInit();
    KDogWatchLog("GDT Initialized", False);
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
    TTYUPrint(
        "\n"
        "$!EBosyOS control codes$!F: $*F$!0See *mezex.txt*$*0$!F\n"
        "$!ERun a test application$!F.\n");
    // programtest();
    TTYUPrint(
        "$!Aapplication skiped$!F.\nTry press F-keys\n");
    Bool lk = False;
    KDogWatchPStart(0, "Main loop");
    for (;;) {
        if ((KBState.SC & 0x80) && !lk) {
            lk = True;
            if (KBState.Key == F1) {
                KPanic("You call, Your problem", True);
            }
            if (KBState.Key == F4) {
                TTYUPrintHex(calculate_shash(PITTime));
            }
            if (KBState.Key == F5) {
                WordGen();
            }
            if (KBState.Key == F2) {
                TTYCursor = 0;
                TTYClear();
                const String data = "                              The $!EBosyOS$!F Main page\x0a$!B\x0a    Links:\x0a        $!EBosyOS source$!B   - $!9$^https://github.com/Kolya142/BosyOS\x7e$!B\x0a        $!EBosyOS download$!B - $!9$^https://raw.githubusercontent.com/Kolya142/BosyOS/refs/heads/main/release/drive\x7e$!B\x0a    LinksUsefulExternel:\x0a        $!ECP437$!B   - $!9$^https://en.wikipedia.org/wiki/Code_page_437\x7e$!B\x0a        $!EVgaText$!B - $!9$^https://en.wikipedia.org/wiki/VGA_text_mode\x7e$!B\x0a\x0a    CurrentState:\x0a        Keyboard input & Vga($!E80x25 16color$!B) output\x0a        $!E1$!B core $!E1$!B process execution\x0a\x0a    Mezex:\x0a        support - BosyOS TTY library & html(compiler)\x0a        usage:\x0a            $!E\$!$!FV$!A(V - hexdigit) $!E-$!B set foreground in vga color\x0a            $!E\$*$!FV$!A(V - hexdigit) $!E-$!B set background in vga color\x0a            $!E\\\$                $!E-$!B write \$ sign\x0a            $!E\\\\                $!E-$!B write \\ symbol\x0a            $!E\\n                $!E-$!B write newline symbol\x0a            $!E";
                TTYUPrint(data);
                TTYCursor = 0;
            }
            if (KBState.Key == F3) {
                
                KDogWatchPStart(0, "Music Player");

                U16 tones[] = { 3, 105, 91, 0, 91, 94, 0, 94, 90, 0, 90, 95, 0, 96, 90, 0, 90, 94, 0, 94, 90, 0, 90, 96, 0, 0, 198, 186, 184, 0, 0, 186, 186, 184, 0, 0, 184, 187, 198, 0, 0, 186, 184, 186, 0, 0, 186, 184, 0, 0, 184, 187, 0, 0, 198, 186, 184, 0, 0, 186, 186, 184, 0, 0, 184, 187, 198, 186, 184, 186, 186, 184, 184, 187, 0, 0, 198, 186, 184, 0, 0, 186, 186, 184, 0, 0, 184, 187, 0, 0, 198, 186, 184, 0, 0, 186, 186, 184, 0, 0, 184, 187, 0, 0, 198, 186, 184, 0, 0, 186, 186, 184, 184, 187, 198, 186, 184, 186, 186, 184, 184, 187, 198, 186, 184, 186, 0, 186, 0, 184, 184, 0, 187, 0, 198, 186, 0, 184, 0, 186, 186, 0, 184, 184, 187, 198, 186, 184, 0, 186, 0, 186, 0, 184, 184, 0, 187, 0, 198, 186, 0, 184, 0, 186, 186, 0, 184, 184, 187, 198, 186, 184, 186, 0, 186, 0, 184, 184, 0, 187, 0, 198, 186, 0, 184, 0, 186, 186, 0, 184, 184, 187, 198, 186, 184, 0, 186, 0, 186, 0, 184, 184, 0, 187, 0, 198, 186, 0, 184, 0, 186, 186, 0, 184, 184, 187, 198, 186, 184, 186, 0, 186, 0, 184, 184, 0, 187, 0, 198, 186, 0, 184, 0, 186, 186, 0, 184, 184, 187, 198, 186, 184, 0, 186, 0, 186, 0, 184, 184, 0, 187, 0, 198, 186, 0, 184, 0, 186, 186, 0, 184, 184, 187, 198, 186, 184, 186, 0, 186, 0, 184, 184, 0, 187, 0, 198, 186, 0, 184, 0, 186, 186, 0, 184, 184, 187, 198, 186, 184, 0, 186, 0, 186, 0, 184, 184, 0, 187, 0, 198, 186, 0, 184, 0, 186, 186, 0, 184, 0, 184, 0, 187, 0, 0, 0, 0, 569, 0, 0, 0, 558, 0, 0, 0, 570, 0, 0, 0, 557, 0, 0, 371, 0, 0, 0, 184, 187, 0, 0, 0, 0, 569, 0, 0, 0, 558, 0, 0, 0, 570, 0, 0, 0, 557, 0, 0, 0, 186, 184, 0, 0, 0, 0, 184, 187, 0, 0, 0, 0, 569, 0, 0, 0, 558, 0, 0, 0, 570, 0, 0, 0, 557, 0, 0, 0, 371, 0, 0, 0, 184, 187, 0, 0, 0, 0, 569, 0, 0, 0, 558, 0, 0, 0, 372, 198, 0, 0, 186, 184, 186, 0, 0, 186, 184, 0, 0, 184, 187, 198, 186, 184, 186, 0, 186, 0, 184, 184, 0, 187, 0, 198, 186, 0, 184, 0, 186, 186, 0, 184, 184, 187, 198, 186, 184, 0, 186, 0, 186, 0, 184, 184, 0, 187, 0, 198, 186, 0, 184, 0, 186, 186, 0, 184, 184, 187, 198, 186, 184, 186, 0, 186, 0, 184, 184, 0, 187, 0, 198, 186, 0, 184, 0, 186, 186, 0, 184, 184, 187, 198, 186, 184, 0, 186, 0, 186, 0, 184, 184, 0, 187, 0, 198, 186, 0, 184, 0, 186, 186, 0, 184, 184, 187, 198, 186, 184, 186, 0, 186, 0, 184, 184, 0, 187, 0, 198, 186, 0, 184, 0, 186, 186, 0, 184, 184, 187, 198, 186, 184, 0, 186, 0, 186, 0, 184, 184, 0, 187, 0, 198, 186, 0, 184, 0, 186, 186, 0, 184, 184, 187, 198, 186, 184, 186, 0, 186, 0, 184, 184, 0, 187, 0, 198, 186, 0, 184, 0, 186, 186, 0, 184, 184, 187, 198, 186, 184, 0, 186, 0, 186, 0, 184, 184, 0, 187, 0, 198, 186, 0, 184, 0, 186, 186, 0, 184, 0, 184, 0, 187, 0, 0, 0, 0, 569, 0, 0, 0, 558, 0, 0, 0, 570, 0, 0, 0, 557, 0, 0, 371, 0, 0, 0, 184, 187, 0, 0, 0, 0, 569, 0, 0, 0, 558, 0, 0, 0, 570, 0, 0, 0, 557, 0, 0, 0, 186, 184, 0, 0, 0, 0, 184, 187, 0, 0, 0, 0, 569, 0, 0, 0, 558, 0, 0, 0, 570, 0, 0, 0, 557, 0, 0, 0, 371, 0, 0, 0, 184, 187, 0, 0, 0, 0, 569, 0, 0, 0, 558, 0, 0, 0, 372, 198, 0, 0, 186, 184, 186, 0, 0, 186, 184, 0, 0, 184 };
                TTYUPrint("$!0$*A<Song> $!F$*0");
                U8 tone = 0;

                U32 z = calculate_shash(PITTime);

            
                for (;;) {
                    KDogWatchPTick(0);
                    U16 t = tones[tone];

                    TTYRawPrint("012345679ABCDEF"[(t >> 8) >> 8], Black, White);
                    TTYRawPrint("012345679ABCDEF"[(t >> 8) & 15], Black, White);
                    TTYRawPrint("012345679ABCDEF"[(t & 0xff) >> 8], Black, White);
                    TTYRawPrint("012345679ABCDEF"[(t & 0xff) & 15], Black, White);
                    TTYCursor -= 4;

                    U32 c = TTYCursor;
                    TTYCursor += 80 - TTYCursor % 80;
                    WordGen();
                    TTYCursor = c;
                    
                    if (t != 0) {
                        Beep(t);
                    }
            
                    MSleep(60);
                    tone++;
                    vga[(z >> 16) & 0xfff] = z & 0xffff;
                    z = calculate_shash((c >> 1) ^ PITTime);
                    if (tone >= (sizeof(tones) / sizeof(U16))) {
                        break;
                    }
                }
            }  
        }
        else if (!(KBState.SC & 0x80) && lk) {
            lk = False;
        }
        KDogWatchPTick(0);
    }
    KDogWatchPEnd(0);
    PowerOff();
}

U0 SysCallTest() {
    U32 addr = RegGet(REG_ESI);
    if (addr > UserSegment.length)
        return;
    addr += (U32)UserSegment.addr;
    // TTYRawPrint("012345679ABCDEF"[(addr >> 8) >> 8], Black, White);
    // TTYRawPrint("012345679ABCDEF"[(addr >> 8) & 15], Black, White);
    // TTYRawPrint("012345679ABCDEF"[(addr & 0xff) >> 8], Black, White);
    // TTYRawPrint("012345679ABCDEF"[(addr & 0xff) & 15], Black, White);
    const String hex = "0123456789ABCDEF";
    TTYUPrint((Char*)addr);
}

U0 programtest()
{
    U8 *app = "BOSY\"\x00\x00\x00f\xb8\x01\x00\xbe\r\x00\x00\x00\xcdx\xeb\xfe$!AHello, world!$!F\n\x00";
    BsfApp bapp = BsfFromBytes(app);
    UserSegment = (Segment) {
        .addr = bapp.data,
        .length = bapp.header.CodeS,
    };
    BsfExec(&bapp);
}
