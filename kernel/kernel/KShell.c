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
#include <fs/vfs.h>

U32 syscall(U32 id, U32 a, U32 b, U32 c, U32 d);
U0 print(String text);
U32 read(String buf, U32 count) {
    return syscall(3, 0, (U32)buf, count, 0);
}
U0 print_dec(U32 a) {
    if (!a) {
        print("0");
        return;
    }
    Char buf[11] = {0};
    U32 p = 10;
    buf[p] = 0;

    while (a) {
        buf[--p] = '0' + (a % 10);
        a /= 10;
    }

    for (U32 i = 0; i < 11; ++i) {
        if (buf[i]) {
            syscall(4, 1, (U32)&buf[i], 1, 0);
        }
    }
}
U0 print_deci(I32 a) {
    if (a < 0) {
        print("-");
        a = -a;
    }
    print_dec(a);
}
U0 printf(String fmt, ...) {
    va_list args;
    va_start(args, fmt);
    U32 n = 4;
    while (*fmt) {
        if (*fmt == '%' && *(fmt + 1)) {
            ++fmt;
            if (*fmt >= '0' && *fmt <= '4') {
                n = *fmt - '0';
                ++fmt;
            }
            switch (*fmt) {
                case 'c': {
                    Char c = va_arg(args, U32);
                    syscall(4, 1, (U32)&c, 1, 0);
                } break;
                case 'C': {
                    Char c = UpperTo(va_arg(args, U32));
                    syscall(4, 1, (U32)&c, 1, 0);
                } break;
                case 's': {
                    String s = va_arg(args, String);
                    if (!s) s = "(null)";
                    print(s);
                } break;
                case 'B': {
                    Bool s = va_arg(args, U32);
                    if (s) {
                        print("True");
                    }
                    else {
                        print("False");
                    }
                } break;
                case 'b': {
                    U16 s = va_arg(args, U32);
                    for (I8 i = n * 8 - 1; i >= 0; --i) {
                        Char c = (s & (1 << i)) ? '1' : '0';
                        syscall(4, 1, (U32)c, 1, 0);
                    }
                } break;                
                case 'd': {
                    U32 s = va_arg(args, U32);
                    print_dec(s);
                } break;
                case 'i': {
                    I32 s = va_arg(args, I32);
                    print_deci(s);
                } break;
                case 'p': {
                    U32 s = va_arg(args, U32);
                    print("0x");
                } break;
                case 'z': {
                    U32 s = va_arg(args, U32);
                } break;
                case 'x': {
                    U32 s = va_arg(args, U32);
                    for (U8 i = n * 2; i > 0; --i) {
                        Char c = "0123456789abcdef"[(s >> ((i - 1) * 4)) & 0xF];
                        syscall(4, 1, (U32)&c, 1, 0);
                    }
                } break;
                case 'X': {
                    U32 s = va_arg(args, U32);
                    for (U8 i = n * 2; i > 0; --i) {
                        Char c = "0123456789ABCDEF"[(s >> ((i - 1) * 4)) & 0xF];
                        syscall(4, 1, (U32)&c, 1, 0);
                    }
                } break;
                case '%': {
                    print("%");
                } break;
                default:
                    print("%");
                    syscall(4, 1, (U32)fmt, 1, 0);
            }
            n = 4;
            ++fmt;
            continue;
        }
        syscall(4, 1, (U32)fmt, 1, 0);
        ++fmt;
    }
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

U0 lsfunc(String name, VFSStat *stat) {
    printf("File: %s\n", name);
}

U0 termrun(const String cmd) {
    if (cmd[0] == '&') {
        c = cmd+1;
        PrintF("not supported\n");
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
        printf("Time: %d\n", BosyTime);
    }
    else if (!StrCmp(cmd, "help")) {
        print("tut - tutorial\n");
        print("time - current time\n");
        print("cls - clear screen\n");
        print("ls - list files\n");
        print("help - show this\n");
        print("&CMD - run CMD in new task\n");
        print("!FILE - run bsfexe file FILE\n");
    }
    else if (!StrCmp(cmd, "ls")) {
        VFSReadDir(lsfunc);
    }
    else if (StrStartsWith(cmd, "cat")) {
        U8 buf[50] = {0};
        printf("readed: %dB\nfile content:\n%s\n", VFSRead(cmd + 4, buf, 0, 50), buf);
    }
    else if (StrStartsWith(cmd, "!")) {
        U8 buf[2048] = {0};
        VFSRead(cmd+1, buf, 0, 2048);
        BsfApp app = BsfFromBytes(buf);
        BsfExec(&app, 0, 1);
    }
    else if (!StrCmp(cmd, "cls")) {
        U32 width;
        U32 height;
        syscall(54, 1, 0, (U32)&width, (U32)&height);
        for (U32 i = 0; i < width*height; ++i) {
            print(" ");
        }
        print((U8[]) {ASCIIPHome, 0});
    }
    else {
        print("Unk ");
        print(cmd);
        print("\n");
    }
}