// Kernel
#include <kernel/KDogWatch.h>
#include <kernel/KWinDemo.h>
#include <kernel/KTasks.h>
#include <kernel/KPanic.h>
#include <kernel/KSys.h>
#include <kernel/KMem.h>

// Drivers
#include <drivers/input/keyboard.h>
#include <drivers/network/rtl8139.h>
#include <drivers/serial/serial.h>
#include <drivers/input/mouse.h>
#include <drivers/misc/random.h>
#include <drivers/video/vga.h>
#include <drivers/disk/ide.h>
#include <drivers/time/pit.h>
#include <drivers/sys/pci.h>
#include <drivers/input/ps2.h>
#include <drivers/sys/beep.h>

// Miscellaneous
#include <misc/driverreg.h>
#include <misc/vdrivers.h>
#include <lib/sys/syscall.h>
#include <misc/wordgen.h>

// Lang
#include <lang/Tokenizer.h>

// KWS
#include <kws/win.h>

// Libraries
#include <lib/IO/KeyboardLib.h>
#include <lib/graphics/Graphics.h>
#include <lib/memory/Collects.h>
#include <lib/strings/String.h>
#include <lib/memory/MemLib.h>
#include <lib/time/Time.h>
#include <lib/IO/BosZ.h>
#include <lib/formats/ELF.h>
#include <lib/formats/bsfexe.h>
#include <lib/IO/TTY.h>
#include <lib/net/IP.h>

// FileSystem
#include <fs/minix.h>
#include <fs/ramfs.h>
#include <fs/vfs.h>

// Arch/Cpu Functions
#include <arch/x86/sys/paging.h>
#include <arch/x86/sys/ring3.h>
#include <arch/x86/cpu/cpu.h>
#include <arch/x86/sys/gdt.h>
#include <arch/x86/cpu/fpu.h>
#include <arch/x86/sys/sys.h>
#include <arch/x86/cpu/pic.h>

U32 syscall(U32 id, U32 a, U32 b, U32 c, U32 d);
U0 print(String text);
U32 read(String buf, U32 count) {
    return syscall(3, 0, (U32)buf, count, 0);
}

U0 CHelp() {
    print(
        "$!5Commands$!F:\n"
        "$!dtut$!F\n"
        "$!Ahelp cls hlt gen testcom$!F\n"
        "$!Btime echo wsave rand testfpu$!F\n"
        "$!Creboot poweroff apple testdrv$!F\n"
        "$!Esnake text sound wget genpass$!F\n"
        "$!Awords triangle bf hz jump touchr$!F\n"
        "$!Bmus pong testufs testheap$!F\n"
        "$!Ctestbfs ls cat fwrite testip$!F\n"
        "$!Etouch fclean collects mouse$!F\n"
        "$!Aboot vrm rand1 stat pass deb task$!F\n"
        "$!5Features$!F:\n"
        "Use '$!7&$!F' at beginning of line to run in new process\n"
        "All Commands: esc-exit\n"
        "notes:\n"
        "change tty with keys $!BF1$!F-$!BF4$!F"
    );
}
static String c;

U0 termrun(const String cmd);

U0 termrun(const String cmd) {
    if (cmd[0] == '&') {
        c = cmd+1;
        PrintF("currently not supported\n");
    }
    else if (!StrCmp(cmd, "tut")) {
        // print("Welcome to $!BBosyOS$!F\n");
        // print("Learning steps:\n");
        // print("1. $!EMezex$!F, $!EMezex$!F terminal\n");
        // print("2. $!7Utils$!F\n");
        // print("3. $!6Development for $!BBosyOS$!F\n");
        // Char c;
        // KWSInputData inp;
        // KWSUpdate(&inp);
        // {
        //     print("Press $!dSPACE$!F to start\n");
        //     while (!inp.key_pressed[' ']) KWSUpdate(&inp);
        //     KWSUpdate(&inp);
        //     while (syscall(3, 0, (U32)&c, 1, 0));

        //     print("1. Mezex\n");
        //     print("$!EMezex$!F uses $!EVga$!F color codes\n");
        //     print("$!EMezex$!F avaliable in $!BBosyOS$!F, $!EHTML$!F, Linux util \"$!7mez$!F\"\n");
        //     print("Vga colors:\n");
        //     print("0 - $!0$*FBlack$!F$*0\n");
        //     print("1 - $!1DBlue$!F\n");
        //     print("2 - $!2DGreen$!F\n");
        //     print("3 - $!3DCyan$!F\n");
        //     print("4 - $!4DRed$!F\n");
        //     print("5 - $!5DPurple$!F\n");
        //     print("6 - $!6DBrown$!F\n");

        //     print("7 - $!7Gray$!F\n");
        //     print("8 - $!8DGray$!F\n");

        //     print("9 - $!9Blue$!F\n");
        //     print("A - $!AGreen$!F\n");
        //     print("B - $!BCyan$!F\n");
        //     print("C - $!CRed$!F\n");
        //     print("d - $!dPurple$!F\n");
        //     print("E - $!EBrown$!F\n");
        //     print("F - White\n");

        //     print("\nCodes:\n");
        //     print("\\$!V - set foreground color to V\n");
        //     print("\\$*V - set background color to V\n");
        //     print("\\$H \\$D - SetHome, MoveDown\n");
        //     print("\\$U \\$L \\$R - MoveUp, MoveLeft, MoveRight\n");
        // }

        // {
        //     print("Press $!dSPACE$!F to continue\n");
        //     while (!inp.key_pressed[' ']) KWSUpdate(&inp);
        //     KWSUpdate(&inp);
        //     while (syscall(3, 0, (U32)&c, 1, 0));

        //     print("2. Utils\n");

        //     print("$!BBosyOS$!F supports 4 tty buffers\n");

        //     print("Press $!dF4$!F to open debug menu\n");
        //     while (!KBState.keys[ASCIIPF4]);

        //     print("\nYou see some KDogWatch logs\n");

        //     print("Press $!dF1$!F to return\n");
        //     while (!KBState.keys[ASCIIPF1]);
        // }

        // {
        //     print("Press $!dSPACE$!F to continue\n");
        //     while (!inp.key_pressed[' ']) KWSUpdate(&inp);
        //     KWSUpdate(&inp);
        //     while (syscall(3, 0, (U32)&c, 1, 0));

        //     print("3. Development for $!BBosyOS$!F\n");

        //     print("Currently $!BBosyOS$!F doen't support development\n");
        // }

        // while (syscall(3, 0, (U32)&c, 1, 0));
    }
    else if (!StrCmp(cmd, "time")) {
        print("// TODO: make printf\n");
    }
    else if (!StrCmp(cmd, "help")) {
        print("tut - tutorial\n");
        print("time - current time\n");
        print("cls - clear screen\n");
        print("help - show this\n");
        print("&CMD - run CMD in new task\n");
    }
    else if (!StrCmp(cmd, "cls")) {
        U32 width;
        U32 height;
        syscall(54, 1, 0, (U32)&width, (U32)&height);
        for (U32 i = 0; i < width*height; ++i) {
            print(" ");
        }
        print((U8[]) {ASCIIPHome});
    }
    else {
        print("Unk ");
        print(cmd);
        print("\n");
    }
}