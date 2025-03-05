// Kernel
#include <kernel/KDogWatch.h>
#include <kernel/KTasks.h>
#include <kernel/KPanic.h>
#include <kernel/KMem.h>

// Drivers
#include <drivers/keyboard.h>
#include <drivers/rtl8139.h>
#include <drivers/serial.h>
#include <drivers/mouse.h>
#include <drivers/vga.h>
#include <drivers/ide.h>
#include <drivers/pit.h>

// Miscellaneous
#include <misc/driverreg.h>
#include <misc/vdrivers.h>
#include <misc/syscall.h>
#include <misc/wordgen.h>
#include <misc/bsfexe.h>

// Libraries
#include <lib/KeyboardLib.h>
#include <lib/Graphics.h>
#include <lib/Collects.h>
#include <lib/String.h>
#include <lib/Random.h>
#include <lib/MemLib.h>
#include <lib/Time.h>
#include <lib/BosZ.h>
#include <lib/TTY.h>
#include <lib/IP.h>

// FileSystem
#include <fs/minix.h>
#include <fs/ramfs.h>

// Arch/Cpu Functions
#include <arch/paging.h>
#include <arch/getreg.h>
#include <arch/ring3.h>
#include <arch/beep.h>
#include <arch/cpu.h>
#include <arch/gdt.h>
#include <arch/fpu.h>
#include <arch/sys.h>
#include <arch/pic.h>

U0 programtest();
// extern U0 KernelDebug();
U0 mainloop();
U0 loop();

__attribute__((naked)) U0 KernelMain() {
    TTYSwitch(TTYC_VGA);
    VgaInit();
    TTYClear();
    TTYCursor = 0;

    HeapInit();
    KDogWatchInit();
    GDTInit();
    IDTInit();
    PICMap();
    PITInit();
    VgaGraphicsSet();
    TTYSwitch(TTYC_320);
    TTYClear();
    TTYCursor = 0;
    KDogWatchLog("System initializing start", False);

    U16 mem = MemorySize();
    PrintF("$!EDetected Memory size $!C- $!B0x%2xKB$!F\n\n", mem);
    if (mem < 64000) {
        KDogWatchLog("$!CWarning: Memory size < $!B64MB$!F", False);
    }

    VgaBlinkingSet(False);
    VgaCursorDisable();
    VgaCursorEnable();
    KDogWatchLog("PIC PIT Vga Initialized", False);
    // VgaGraphicsSet();
    // VRMClear(Purple);
    SysCallInit();
    SysCallSetup();
    
    KDogWatchLog("SysCalls Initialized", False);
    
    PagingInit();
    
    U32 cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    if (!(cr0 & 0x80000000)) {
        KDogWatchLog("Paging is NOT enabled!", True);
    }

    KDogWatchLog("Initialized \x9Bpaging\x9C", False);

    KDogWatchLog("Setuping fpu", False);
    FPUBox();
    
    // Drivers
    KDogWatchLog("Setuping drivers", False);
    SerialInit();
    KDogWatchLog("Initialized \x9Bserial\x9C", False);
    // RTL8139Init();
    // KDogWatchLog("Initialized \"rtl8139\"", False);
    KBInit();
    KDogWatchLog("Initialized \x9Bkeyboard\x9C", False);
    // MouseInit(); // Portal to hell
    // KDogWatchLog("Initialized \"mouse\"", False);
    BeepInit();
    KDogWatchLog("Initialized \x9Bpc speaker\x9C", False);
    IDEInit();
    KDogWatchLog("Initialized \x9Bide disk\x9C", False);
    VDriversReg();
    KDogWatchLog("Initialized \x9Bvdrivers\x9C", False);
    // Drivers end
    
    // FSs
    KDogWatchLog("Setuping FileSystems", False);
    RFSInit();
    KDogWatchLog("Initialized \x9Bramfs\x9C", False);
    MXInit();
    KDogWatchLog("Initialized \"minix fs\"", False);
    // DATInit();
    // KDogWatchLog("Initialized \"dat\"", False);
    // BOTFSInit();
    // KDogWatchLog("Initialized \"botfs\"", False);
    // UFSInit();
    // KDogWatchLog("Initialized \"ufs\"", False);
    // VFSInit();
    // KDogWatchLog("Initialized \"vfs\"", False);

    KDogWatchLog("System Initialized", False);
    // TTYClear();
    // TTYCursor = 0;

    TSleep(500);
    U32 tid1 = TaskNew((U32)mainloop, 0x10, 8);
    // mainloop();
    // TaskNew((U32)mainloop);
    // TaskNew((U32)loop);
    // TaskingIs = True;
    for(;;);
    CpuHalt();
}
// INT_DEF(KernelDebug) {
//     U32 c = TTYCursor;
//     TTYSwitch(TTYC_SER);
//     TTYCursor = 0;
//     PrintF("EIP: %08x ESP: %08x EBP: %08x\n", regs->eip, regs->useresp, regs->ebp);
//     PrintF("EAX: %08x EBX: %08x ECX: %08x EDX: %08x\n", regs->eax, regs->ebx, regs->ecx, regs->edx);
//     TTYSwitch(TTYC_RES);
//     TTYCursor = c;
// }
U0 CHelp() {
    TTYUPrint(
        "$!5Commands$!F:\n"
        "$!Ahelp cls hlt gen testcom$!F\n"
        "$!Btime echo wsave rand testfpu$!F\n"
        "$!Creboot poweroff apple testdrv$!F\n"
        "$!Esnake text sound wget$!F\n"
        "$!3words triangle bf hz$!F\n"
        "$!5mus pong testufs testheap$!F\n"
        "$!6testbfs ls cat fwrite testip$!F\n"
        "$!7touch fclean collects mouse$!F\n"
        "$!8boot vrm rand1 stat pass deb$!F"
        "$!5Features$!F:\n"
        "All Commands: esc-exit\n"
        "$!Asound$!F - $!Balt$!F-play, $!Blshift$!F-save, $!Brshift$!F-load\n"
        "$!Awsave$!F/$!Awget$!F uses disk\n"
        "use $!BWASD$!F in $!Atext$!F to move cursor"
    );
}
U0 CCls() {
    TTYClear();
    TTYCursor = 0;
}
U0 CWords() {
    TTYClear();
    TTYCursor = 0;
    while (TTYCursor < 80*20) {
        PrintF("%w");
    }
    TTYUPrintC('\n');
}
U0 CTriangle() {
    TTYClear();
    TTYCursor = 0;
    Bool state[25] = {0};
    state[0] = True;
    for (;!KBState.keys['\x1b'];) {
        for (U32 i = 25; i > 0; --i) {
            if (state[i-1]) {
                state[i] = !state[i];
            }
        }
        U32 count = 0;
        for (U32 j = 0; j < 25; ++j) {
            count += state[j];
            VgaPSet(0, 25 - j, state[j] ? 0xDB : ' ', 0x0F);
            for (U32 i = 80; i > 0; --i) {
                VgaPSet(i, 25 - j, vga[i - 1 + (25 - j) * 80] & 0xff, 0x0F);
            }
        }
        BeepSPC(count + 30, 30);
        BeepSPC(count + 50, 10);
        BeepSPC(count + 70, 5);            
        TSleep(20);
        KDogWatchPTick(0);
    }
}
U0 CTime() {
    PrintF("$!BPIT$!F:    %x\n", PITTime);
    PrintF("$!BYear$!F:   %d\n", SystemTime.year);
    PrintF("$!BMonth$!F:  %d\n", SystemTime.month);
    PrintF("$!BDay$!F:    %d\n", SystemTime.day);
    PrintF("$!BHour$!F:   %d\n", SystemTime.hour);
    PrintF("$!BMinute$!F: %d\n", SystemTime.minute);
    PrintF("$!BSecond$!F: %d\n", SystemTime.second);
}
U0 CRand() {
    Bool lk = False;
    I16 note = 60;
    KDogWatchPEnd(0);
    U32 c = TTYCursor;
    U32 v = 50542;
    for (U32 i = 0; !KBState.keys['\x1b']; ++i) {
        note += ((I32)RandomU() % 6) - 3;
        if (note < 20) note = 20;
        if (note > 150) note = 150;
        U16 dur = RandomU() % 150 + 50;
        BeepSPC(note, dur);
        vga[(v >> 16) % 1999] = v & 0xffff;
        v = calculate_shash(v * 50239 + note ^ dur);
        if (RandomU() % 5 <= 2) TSleep(RandomU() % 200 + 100);
        else if (RandomU() % 4 <= 1) {
            BeepSPC(note + 4, dur);
            BeepSPC(note + 7, dur);
        }
        else if (RandomU() % 5 == 1) {
            BeepSPC(note + 4, dur);
            BeepSPC(note + 7, dur);
            BeepSPC(note + 11, dur);
            BeepSPC(note + 13, dur);
        }
        WordGen();
        if (!(i % 5))
            TTYCursor = c;
        TSleep(1000/120); // 120gh
    }
}
U0 CGen() {
    Bool lk = False;
    for (U32 i = 0; !KBState.keys['\x1b']; ++i) {
        for (U32 x1 = 0; x1 < 320; ++x1) {
            for (U32 y1 = 0; y1 < 200; ++y1) {
                VRMPSet(x1, y1, (calculate_shash((x1+PITTime / 100)^y1)) & 15);
            }
        }
        KDogWatchPTick(0);
        TSleep(1000/30);
    }
}
U0 CMus() {
    U16 tones[] = { 79, 78, 76, 76, 78, 57, 62, 66, 62, 59, 62, 66, 62, 57, 62, 66, 69, 62, 79, 59, 78, 62, 76, 66, 76, 62, 57, 78, 62, 66, 62, 74, 59, 62, 76, 66, 69, 62, 57, 62, 66, 62, 59, 62, 66, 69, 62, 76, 59, 64, 78, 67, 79, 64, 59, 64, 76, 67, 73, 64, 57, 74, 64, 67, 64, 76, 57, 64, 69, 67, 69, 64, 57, 78, 62, 66, 62, 59, 62, 66, 62, 57, 62, 66, 62, 79, 59, 78, 62, 76, 66, 76, 62, 78, 57, 62, 66, 62, 59, 62, 66, 62, 57, 62, 66, 69, 62, 79, 59, 78, 62, 76, 66, 76, 62, 57, 62, 78, 66, 74, 62, 59, 62, 76, 66, 69, 62, 57, 62, 66, 62, 59, 62, 66, 62, 76, 59, 64, 78, 67, 79, 64, 59, 64, 76, 67, 73, 64, 57, 64, 74, 67, 76, 64, 57, 69, 64, 74, 67, 76, 64, 77, 58, 76, 62, 74, 65, 72, 70, 69, 70, 72, 53, 60, 77, 65, 60, 76, 55, 74, 62, 74, 65, 72, 62, 74, 57, 72, 60, 72, 65, 60, 72, 53, 60, 69, 65, 70, 60, 72, 53, 60, 77, 65, 60, 79, 55, 77, 62, 76, 65, 74, 62, 74, 57, 76, 60, 77, 65, 60, 77, 57, 60, 79, 65, 81, 60, 82, 58, 82, 65, 81, 69, 65, 79, 58, 65, 77, 69, 79, 65, 81, 57, 81, 60, 79, 65, 60, 77, 57, 60, 74, 65, 72, 60, 74, 57, 77, 60, 77, 65, 76, 60, 57, 76, 60, 78, 64, 78, 66, 50, 57, 78, 74, 50, 57, 71, 47, 54, 74, 78, 74, 47, 54, 69 };
    U16 durations[] = { 1010, 249, 246, 246, 249, 0, 264, 248, 246, 249, 249, 246, 246, 249, 264, 248, 246, 0, 249, 0, 249, 0, 246, 0, 246, 0, 249, 264, 0, 248, 246, 249, 0, 249, 246, 0, 246, 0, 249, 264, 248, 246, 249, 249, 246, 246, 0, 249, 0, 264, 248, 0, 246, 0, 249, 249, 246, 0, 246, 0, 249, 264, 0, 248, 246, 249, 0, 249, 246, 0, 246, 0, 249, 264, 0, 248, 246, 249, 249, 246, 246, 249, 264, 248, 246, 249, 0, 249, 0, 246, 0, 246, 0, 249, 0, 264, 248, 246, 249, 249, 246, 246, 249, 264, 248, 246, 0, 249, 0, 249, 0, 246, 0, 246, 0, 249, 264, 248, 0, 246, 0, 249, 249, 246, 0, 246, 0, 249, 264, 248, 246, 249, 249, 246, 246, 249, 0, 264, 248, 0, 246, 0, 249, 249, 246, 0, 246, 0, 249, 264, 248, 0, 246, 0, 249, 249, 0, 246, 0, 246, 0, 249, 0, 264, 0, 248, 0, 246, 0, 744, 246, 249, 0, 264, 248, 0, 246, 249, 0, 249, 0, 246, 0, 246, 0, 249, 0, 264, 0, 248, 0, 246, 249, 0, 249, 246, 0, 246, 0, 249, 0, 264, 248, 0, 246, 249, 0, 249, 0, 246, 0, 246, 0, 249, 0, 264, 0, 248, 0, 246, 249, 0, 249, 246, 0, 246, 0, 249, 0, 264, 0, 248, 0, 246, 249, 0, 249, 246, 0, 246, 0, 249, 0, 264, 0, 248, 0, 246, 249, 0, 249, 246, 0, 246, 0, 249, 0, 264, 0, 248, 0, 246, 0, 249, 249, 0, 246, 0, 246, 0, 249, 0, 512, 246, 0, 0, 249, 0, 0, 249, 246, 246, 0, 0, 249 };

    for (U32 i = 0; i < sizeof(tones) / 2; ++i) {
        BeepSPC(tones[i], durations[i]);
        TSleep(1000/120);
        KDogWatchPTick(0);
        if (KBState.keys['\x1b']) {
            break;
        }
    }
    TaskClose();
}


Bool Debugging = True;
U0 CSound() {
    static const U8 KeyToNote[128] = {
        40, 42, 44, 45, 47, 49, 51, 52, 54, 56, 57, 59,
        60, 62, 64, 65, 67, 69, 71, 72, 74, 76, 77, 79,
        81, 83, 85, 87, 89, 91, 93, 95,
        40, 42, 44, 45, 47, 49, 51, 52, 54, 56, 57, 59,
        60, 62, 64, 65, 67, 69, 71, 72, 74, 76, 77, 79,
        81, 83, 85, 87, 89, 91, 93, 95,
        40, 42, 44, 45, 47, 49, 51, 52, 54, 56, 57, 59,
        60, 62, 64, 65, 67, 69, 71, 72, 74, 76, 77, 79,
        81, 83, 85, 87, 89, 91, 93, 95,
        40, 42, 44, 45, 47, 49, 51, 52, 54, 56, 57, 59,
        60, 62, 64, 65, 67, 69, 71, 72, 74, 76, 77, 79,
        81, 83, 85, 87, 89, 91, 93, 95
    };
    
    U8 *Record = MAlloc(555);
    MemSet(Record, 1, 555);
    U32 i = 0;
    Bool start = False;
    for (;;) {
        if (KBState.keys['\x1b']) {
            break;
        }
        else if (KBState.keys[ASCIIPAlt]) {
            for (U32 j = 0; j < 555 && Record[j] != 1; ++j) {
                if (Record[j])
                    BeepSPC(Record[j], 3);
                KDogWatchPTick(0);
                TSleep(5);
            }
        }
        else if (!(KBState.SC & 0x80)) {
            if (KBState.Key > 0x80) {
                Record[i >> 2] = 0;
            }
            else {
                U8 note = KeyToNote[KBState.Key%sizeof(KeyToNote)];
                TTYUPrintHex(note);
                BeepSPC(note, 3);
                Record[i >> 2] = note;
                start = True;
            }
        }
        else if ((KBState.SC & 0x80)) {
            Record[i >> 2] = 0;
        }
        TTYUPrintHex(i >> 2);
        if (start) {
            i = (i+1) % (555 << 2);
        }
        TTYCursor -= TTYCursor % 80;
        KDogWatchPTick(0);
        TSleep(3);
    }
}
U0 CSnake() {
    KDogWatchPEnd(0);
    U32 tail[5] = {0};
    tail[0] = TTYWidth * 10 + 39;
    tail[1] = TTYWidth * 10 + 38;
    tail[2] = TTYWidth * 10 + 37;
    tail[3] = TTYWidth * 10 + 36;
    tail[4] = TTYWidth * 10 + 35;
    U32 snake = TTYWidth * 10 + 40;
    U32 apple = RandomU() % 1300 + 500;
    U32 score = 0;
    U8 dir = 4;
    TTYCursor = 0;
    TTYlbg = White;
    for (U32 i = 0; i < TTYWidth * TTYHeight; i++) {
        TTYPuter(' ');
        ++TTYCursor;
    }
    Bool game = True;
    while (game) {
        if (KBState.keys['w']) {
            if (dir != 1)
                BeepSPC(54, 30);
            dir = 1;
        }
        else if (KBState.keys['a']) {
            if (dir != 2)
                BeepSPC(54, 30);
            dir = 2;
        }
        else if (KBState.keys['s']) {
            if (dir != 3)
                BeepSPC(54, 30);
            dir = 3;
        }
        else if (KBState.keys['d']) {
            if (dir != 4)
                BeepSPC(54, 30);
            dir = 4;
        }

        for (U8 i = 4; i >= 1; --i) {
            TTYCursor = tail[i];
            TTYRawPrint(' ', White, White);
            tail[i] = tail[i-1];
        }
        tail[0] = snake;

        if (snake == apple) {
            score++;
            vga[apple] = 0xF02E;
            apple = RandomU() % 1300 + 500;
            BeepSPC(50, 70);
            BeepSPC(30, 40);
            BeepSPC(50, 70);
        }

        if (dir == 1      && ((snake - TTYWidth) < 0xFFFFFFF)) {
            snake -= TTYWidth;
        }
        else if (dir == 2 && (snake - 1) % TTYWidth != TTYWidth - 1) {
            snake -= 1;
        }
        else if (dir == 3 && ((snake + TTYWidth) < TTYWidth * TTYHeight) )  {
            snake += TTYWidth;
        }
        else if (dir == 4 && ((snake + 1) % TTYWidth != 0)) {
            snake += 1;
        }

        for (U8 i = 0; i < 5; ++i) {
            TTYCursor = tail[i];
            TTYRawPrint('#', Black, White);
            // if (tail[i] == snake) {
            //     for (U32 i = 0; i < 2000; ++i) {
            //         vga[i] ^= 0xFF00;
            //     }
            //     TTYCursor = 0;
            //     TTYUPrint("Game over!\n");
            //     game = False;
            //     break;
            // }
        }
        if (KBState.keys['\x1b']) {
            game = False;
        }
        TTYCursor = apple;
        TTYRawPrint(' ', Red, Red);
        TTYCursor = 80 / 2 - 4 / 2;
        TTYUPrintHex(score);
        TSleep(100);
    }
    TTYUPrint("$!F$*0");
}
U0 CPong() {
    KDogWatchPEnd(0);
    I32 p1 = 10;
    I32 p2 = 10;
    I32 ballx = RandomU() % 10 + TTYWidth/2;
    I32 bally = RandomU() % 10 + 10;
    I32 ballvx = (RandomU() & 1) ? 1 : -1;
    I32 ballvy = (RandomU() & 1) ? 1 : -1;
    Bool game = True;
    while (game) {
        TTYClear();
        
        for (U32 y = p1 - 3; y <= p1 + 3; ++y) {
            TTYCursor = y * TTYWidth + 0;
            TTYRawPrint('#', White, Black);
        }
        for (U32 y = p2 - 3; y <= p2 + 3; ++y) {
            TTYCursor = y * TTYWidth + TTYWidth - 1;
            TTYRawPrint('#', White, Black);
        }
        TTYCursor = ballx + bally * TTYWidth;
        TTYRawPrint('@', White, Black);
        ballx += ballvx;
        bally += ballvy;

        if (bally == TTYHeight-1 || bally == 0) {
            ballvy = -ballvy;
            BeepSPC(45, 30);
        }
        if (ballx == 1 && bally >= p1 - 3 && bally <= p1 + 3) {
            ballvx = -ballvx;
            BeepSPC(45, 30);
        }
        if (ballx == TTYWidth-1/* && bally >= p2 - 3 && bally <= p2 + 3*/) {
            ballvx = -ballvx;
            BeepSPC(45, 30);
        }
        if (KBState.keys['w']) {
            --p1;
        }
        if (KBState.keys['s']) {
            ++p1;
        }
        p2 += (bally - p2) / 3;
        if (ballx == TTYWidth-1 || ballx == 0) {
            ballx = RandomU() % 10 + TTYWidth/2;
            bally = RandomU() % 10 + 10;
            ballvx = (RandomU() & 1) ? 1 : -1;
            ballvy = (RandomU() & 1) ? 1 : -1;
            BeepSPC(25, 30);
        }
        if (KBState.keys['\x1b']) {
            game = False;
        }
        TSleep(100);
    }
}
U0 BFInterpret(const String code) {
    U32 stack[32] = {0};
    U8  mem[3355] = {0};
    U32 stacki = 0;
    U32 memi = 0;
    for (U32 i = 0; code[i]; ++i) {
        switch (code[i]) {
            case '+':
                ++mem[memi];
                break;
            case '-':
                --mem[memi];
                break;
            case '<':
                --memi;
                break;
            case '>':
                ++memi;
                break;
            case '[': {
                U32 j = 1;
                if (!mem[memi]) {
                    while (j) {
                        ++i;
                        if (!code[i]) break;
                        if (code[i] == '[') ++j;
                        if (code[i] == ']') --j;
                    }
                }
                else {
                    stack[stacki++] = i;
                }
                }
                break;
            case ']':
                if (mem[memi]) {
                    i = stack[stacki--] - 1;
                }
                else {
                    --stacki;
                }
                break;
            case '.':
                TTYUPrintC(mem[memi]);
                break;
            case '/':
                TTYUPrintDec(mem[memi]);
                break;
        }
    }
}
U0 CHz() {
    KDogWatchPEnd(0);
    for (;;) {
        for (F32 s = 1.; s < 200.; s += 1.) {
            for (F32 o = 0.; o < 50.; o += 1.) {
                F32 fr = 440. * pow(2., (o / 12.)) + (1.0f + s / 200.);
                U32 f = (U32)fr;
                BeepHz(f, f / 6);
                TTYUPrintHex(fr);
                TTYCursor -= 8;
                if (KBState.keys['\x1b']) 
                    break;
            }
            if (KBState.keys['\x1b']) 
                break;
        }
        if (KBState.keys['\x1b']) 
            break;
    }
}
U0 CRay() {
    KDogWatchPEnd(0);
    U8 map[16 * 16] = {
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,
        1,0,1,0,1,0,1,0,1,0,1,1,1,0,0,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,
        1,0,0,0,0,0,0,1,0,0,1,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,
        1,0,0,1,0,0,0,1,0,0,1,0,0,0,0,1,
        1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    };
    F32 px = 1., py = 1.;
    F32 pa = 0;
    F32 FOV = 3.1415926535897 / 2.;
    for (;!KBState.keys['\x1b'];) {
        if (KBState.keys['q']) {
            pa -= 0.1;
        }
        if (KBState.keys['e']) {
            pa += 0.1;
        }
        if (KBState.keys['w']) {
            F32 dx = cos(pa), dy = sin(pa);
            px += dx * 0.1;
            py += dy * 0.1;
        }
        if (KBState.keys['s']) {
            F32 dx = cos(pa), dy = sin(pa);
            px -= dx * 0.1;
            py -= dy * 0.1;
        }
        if (KBState.keys['a']) {
            F32 dx = -sin(pa), dy = cos(pa);
            px -= dx * 0.1;
            py -= dy * 0.1;
        }
        if (KBState.keys['d']) {
            F32 dx = sin(pa), dy = -cos(pa);
            px -= dx * 0.1;
            py -= dy * 0.1;
        }
        for (F32 r = 0; r < 320; ++r) {
            F32 angle = pa + (FOV * ((r / 320.) - .5));
            F32 dx = cos(angle), dy = sin(angle);
            F32 dist = 0.;

            F32 rx = px, ry = py;

            while (dist < 160.) {
                rx += dx * .05;
                ry += dy * .05;
                dist += .05;

                U32 mx = (U32)(rx / 1.), my = (U32)(ry / 1.);
                if (mx >= 16 || my >= 16 || map[my * 16 + mx]) {
                    break;
                }
            }
            I32 height = (U32)(200. / dist);
            for (I32 y = 0; y < 200; ++y) {
                if (y < (100 - height)) {
                    VRMPSet(r, y, 14);
                }
                else if (y > (100 + height)) {
                    VRMPSet(r, y, 15);
                }
                else {
                    VRMPSet(r, y, max(min((U32)(14./dist), 13), 0));
                }
            }
            TTYCursor = 0;
            PrintF("%d,%d", (U32)px, (U32)py);
        }
        TSleep(100);
    }
}
U0 CBoot(String name) {
    U8 *block = MAlloc(512*48);
    
    MXDirEntry ffiles[32] = {0};
    MXListRoot(ffiles, 32);
    for (U32 i = 0; i < 32; ++i) {
        if (!ffiles[i].inode) break;
        if (!StrCmp(ffiles[i].name, name)) {
            MXINode in = MXInodeGet(&MXSB, ffiles[i].inode);
            MXInodeDataRead(&in, block);
            break;
        }
    }
    PrintF("loaded \"%s\" at %p\n", block, block);
    BsfApp app = BsfFromBytes(block);
    I32 code = BsfExec(&app, 0, 1);
    PrintF("Exit code: %d", code-2);
    MFree(block);
}
U32 LazyCalc(U32 x) {
    return !(x & 1) ? (x / 2) : (x * 3 + 1);
}
U0 loop() {
    for (;;) {
        U32 c = TTYCursor;
        TTYCursor = 0;
        TTYPrintC(0x30 + (PITTime % 10));
        TTYCursor = c;
    }
}
typedef struct {
    U16 src_port;
    U16 dst_port;
    U16 length;
    U16 checksum;
} __attribute__((packed)) UDPHeader;
U32 testvar = 0xABAB;
U0 tasking1() {
    for (U32 i = 0; i < 3; ++i) {
        PrintF("tasking %1x\n", i);
        // TaskYeild();
    }
    PrintF("processes end.\nuse task to create new process");
    TaskClose();
}
U0 termrun(const String cmd) {
    if (!StrCmp(cmd, "help")) {
        CHelp();
    }
    else if (!StrCmp(cmd, "cls")) {
        CCls();
    }
    else if (!StrCmp(cmd, "task")) {
        U32 tid2 = TaskNew((U32)tasking1, 0x10, 8);
        PrintF("processes setuped.\nuse tasky to yeild process");
    }
    // else if (!StrCmp(cmd, "tasky")) {
    //     TaskYeild();
    // }
    else if (!StrCmp(cmd, "testip")) {
        NetMac mac;
        DriverCall(0xbb149088, 0xc3442e1e, 2, (U32*)&mac);
        PrintF("%C:%C:%C:%C:%C:%C", mac.w1 >> 8, mac.w1 & 0xFF, mac.w2 >> 8, mac.w2 & 0xFF, mac.w3 >> 8, mac.w3 & 0xFF);

        NetPackage pck;

        U8 payload[] = "12345678Hello, world!";
        UDPHeader udp;
        udp.src_port = HtonW(1234);
        udp.dst_port = HtonW(129);
        udp.length = HtonW(sizeof(payload));
        udp.checksum = 0;
        MemCpy(payload, &udp, sizeof(UDPHeader));


        pck.data = payload;
        pck.dst = (NetMac){0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        pck.length = sizeof(payload)-1;

        pck.ip4.verIhl = 0x45;
        pck.ip4.tos = 0;
        pck.ip4.len = HtonW(sizeof(NetPackageIP4));
        pck.ip4.id = HtonW(0x1234);
        pck.ip4.offset = HtonW(0x4000);
        pck.ip4.ttl = 64;
        pck.ip4.protocol = 0x11;
        pck.ip4.srcip = HtonD(0xC0A80001);  // 192.168.0.1
        pck.ip4.dstip = HtonD(0xC0A81F79);  // 192.168.31.121   

        pck.ip4.checksum = 0;
        pck.ip4.checksum = IPChecksum((U16*)&pck.ip4, sizeof(NetPackageIP4));

        DriverCall(0xbb149088, 0xc3442e1e, 0, (U32*)&pck);
    }
    else if (StrStartsWith(cmd, "boot") && StrCmp(cmd, "boot")) {
        CBoot(&cmd[4]);
    }
    else if (!StrCmp(cmd, "boot")) {
        CBoot("usercode.bin.bsf");
    }
    else if (!StrCmp(cmd, "ray")) {
        CRay();
    }
    else if (!StrCmp(cmd, "testrfs")) {
        PrintF("Creating file\n");
        RFSAdd("test", 50);
        U32 fd = RFSOpen("test");
        PrintF("Writing file\n");
        RFSWrite(fd, "Hello, world!", 14);
        RFSClose(fd);
        PrintF("\n");
        fd = RFSOpen("test");
        Char buf[14];
        PrintF("Reading file\n");
        RFSRead(fd, buf, 14);
        PrintF("Printing file\n");
        RFSClose(fd);
        PrintF("File data: \"%s\"", buf);
    }
    else if (!StrCmp(cmd, "peek")) {
        PrintF("%x", *(U32*)0x0030462a);
    }
    else if (!StrCmp(cmd, "testdrv")) {
        U32 v = 0x00200023;
        DriverCall(0x46ef3f2c, 0x27e134cd, 2, &v);
    }
    else if (!StrCmp(cmd, "testcom")) {
        String str = "The quick brown serial jumps over a lazy vga";
        for (U32 i = 0; str[i]; ++i) {
            U32 b = str[i];
            DriverCall(0xa3f13d05, 0x2eb0f0f, 0, &b);
        }
    }
    else if (!StrCmp(cmd, "testfpu")) {
        F32 x = 0;
        for (;!KBState.keys['\x1b'];) {
            TTYCursor = sin(x) * 20. + 20;
            TTYPrintC(0x30 + (PITTime % 40));
            x += 0.0001;
        }
        CCls();
    }
    else if (!StrCmp(cmd, "deb")) {
        Debugging = !Debugging;
    }
    else if (!StrCmp(cmd, "stat")) {
        Char cpu[49];
        CpuNameGet(cpu);
        PrintF("Width: %d, Height: %d\n", TTYWidth, TTYHeight);
        PrintF("Cpu: %s\n", cpu);
        U16 mem = MemorySize();
        PrintF("memory: 0x%2xKB", mem);
    }
    else if (!StrCmp(cmd, "pass")) {
        PrintF("Enter password: $!A\\$$!0");
        VgaCursorDisable();
        Char buf[20] = {0};
        KBRead(buf, 20);
        VgaCursorEnable();
        PrintF("$!Fpassword: %s", buf);
    }
    else if (!StrCmp(cmd, "rand1")) {
        CCls();
        Char b1[9] = {0};
        Char b2[9] = {0};
        Char b3[9] = {0};

        WordGenS(b1, 9);
        WordGenS(b2, 9);
        WordGenS(b3, 9);

        for (;!KBState.keys['\x1b'];) {
            TTYCursor -= TTYCursor % TTYWidth;
            PrintF("%s and %s and %s - ", b1, b2, b3);
            for (U32 i = 0; i < 8; ++i) {
                BeepSPC(b1[i]+20, RandomU()%30);
                BeepSPC(b2[i]+30, RandomU()%30);
                BeepSPC(b3[i]+40, RandomU()%30);
                PrintF("%c%c%c", b1[i], b2[i], b3[i]);
            }
            WordGenS(b1, 9);
            WordGenS(b2, 9);
            WordGenS(b3, 9);
            TSleep(100);
        }
    }
    else if (!StrCmp(cmd, "vrm")) {
        // PrintF("%p %p\n", GCursor, testvar);
        // for (U32 y = 0; y < 8; ++y) {
        //     for (U32 x = 0; x < 6; ++x) {
        //         PrintF("%c", ".#"[GCursor[x+y*6] != 2]);
        //     }
        //     PrintF("\n");
        // }
        // for(;;);
        // VgaGraphicsSet();
        // VRMClear(Purple);
        // TTYPuter = TTYPutG;
        // TTYWidth = 320  / 6;
        // TTYHeight = 200 / 6;

        Bool state[200] = {0};
        state[0] = True;
        Vec2 cur = vec2(30, 30);
        U8 *buf = MAlloc(320*200);
        for (;!KBState.keys['\x1b'];) {
            for (U32 i = 200; i > 0; --i) {
                if (state[i-1]) {
                    state[i] = !state[i];
                }
            }
            U32 count = 0;
            for (U32 j = 0; j < 200; ++j) {
                count += (state[j]);
                buf[0 + (200 - j) * 320] = state[j] ? White : Black;
                for (U32 i = 320; i > 0; --i) {
                    buf[i + (200 - j) * 320] = buf[i - 1 + (200 - j) * 320] & 0xff;
                }
            }

            MemCpy(VRM, buf, 320*200);

            VRMDrawSprite(cur, vec2(6, 8), Black, Purple, GCursor);

            TTYCursor = 0;
            String text = "The Quick Brown Fox Jumps Over a Lazy Dog0123456789:/.,;`!@ABCDEF HEY Hello, world!";
            PrintF("%s", text);

            if (KBState.keys['w']) --cur.y;
            if (KBState.keys['a']) --cur.x;
            if (KBState.keys['s']) ++cur.y;
            if (KBState.keys['d']) ++cur.x;

            BeepSPC(count + 30, 30);
            BeepSPC(count + 50, 10);
            BeepSPC(count + 70, 5);            
            TSleep(20);
            KDogWatchPTick(0);
        }
    }
    else if (!StrCmp(cmd, "mouse")) {
        for (;!KBState.keys['\x1b'];) {
            CCls();
            TTYCursor = MouseX + MouseY * 80;
            TTYUPrintC(0xB0);
            TSleep(10);
        }
    }
    else if (!StrCmp(cmd, "collects")) {
        FIFO fifo = FIFOInit(1, 10);
        FIFOWrite(&fifo, "t");
        FIFOWrite(&fifo, "1");
        Char c1, c2;
        FIFORead(&fifo, &c1);
        FIFORead(&fifo, &c2);
        PrintF("FIFO: %c%c\n", c1, c2);
        FIFODestroy(&fifo);

        PrintF("LazyList(collatz): ");
        LazyList lazy = LazyListInit(LazyCalc, 20);
        for (;;) {
            U32 elem = LazyListNext(&lazy);
            PrintF("%d ", elem);
            if (elem == 1)
                break;
        }
        PrintF("\n");
        LazyListDestroy(&lazy);
    }
    else if (!StrCmp(cmd, "ls")) {
        MXDirEntry ffiles[32] = {0};
        MXListRoot(ffiles, 32);
        for (U32 i = 0; i < 32; ++i) {
            if (!ffiles[i].inode) break;

            PrintF("$!B%s$!F\n", ffiles[i].name);
        }
    }
    else if (StrStartsWith(cmd, "cat")) {
        MXDirEntry ffiles[32] = {0};
        MXListRoot(ffiles, 32);
        for (U32 i = 0; i < 32; ++i) {
            if (!ffiles[i].inode) break;
            if (!StrCmp(ffiles[i].name, cmd + 4)) {
                MXINode in = MXInodeGet(&MXSB, ffiles[i].inode);
                U8 block[1024];
                MXInodeDataRead(&in, block);
                PrintF("File content:\n%s", block);
                return;
            }
        }
        PrintF("$!CFile not found$!F");
    }
    else if (!StrCmp(cmd, "words")) {
        CWords();
    }
    else if (!StrCmp(cmd, "triangle")) {
        CTriangle();
    }
    // else if (!StrCmp(cmd, "testbfs")) {
    //     BOTFSCreate("test", 10);
    //     BOTFSCreate("trash", 1);
    //     PrintF("File: %B\n", BOTFSFind("test") != 0xFFFFFFFF);
    //     BOTFSWrite("test", "sus\n", 0, 4);
    //     U8 buf[4] = {0};
    //     BOTFSRead("test", buf, 0, 4);
    //     String ffiles[5] = {0};
    //     BOTFSList(ffiles, 5);
    //     PrintF("existing files: ");
    //     for (U32 i = 0; i < 5; ++i) {
    //         if (!ffiles[i]) continue;
    //         PrintF("%s ", ffiles[i]);
    //     }
    //     PrintF("\nData: %s\n", buf);
    //     BOTFSDelete("trash");
    //     PrintF("Deleted trash: %B\n", BOTFSFind("trash") == 0xFFFFFFFF);    
    // }
    else if (!StrCmp(cmd, "testheap")) {
        TTYUPrint("Test:");
        TTYUPrintHex((U32)MAlloc(3));
    }
    else if (!StrCmp(cmd, "hlt")) {
        asmv("cli");
        CpuHalt();
    }
    else if (!StrCmp(cmd, "hz")) {
        CHz();
    }
    else if (!StrCmp(cmd, "time")) {
        CTime();
    }
    else if (StrStartsWith(cmd, "echo ")) {
        TTYUPrint(cmd+4);
    }
    else if (!StrCmp(cmd, "echo")) {
        TTYUPrint(".");
    }
    else if (!StrCmp(cmd, "rand")) {
        CRand();
    }
    else if (!StrCmp(cmd, "gen")) {
        CGen();
    }
    else if (!StrCmp(cmd, "reboot")) {
        PowerReboot();
    }
    else if (!StrCmp(cmd, "poweroff")) {
        KDogWatchPEnd(0);
        PowerOff();
    }
    else if (!StrCmp(cmd, "sound")) {
        CSound();
    }
    else if (!StrCmp(cmd, "snake")) {
        CSnake();
    }
    else if (!StrCmp(cmd, "pong")) {
        CPong();
    }
    else if (!StrCmp(cmd, "$!Csnd$!F")) {
        U16 tones[] = { 61, 59, 57, 55, 55, 57, 59, 61, 63, 64, 64, 65, 66, 66, 65, 64, 63, 62, 64, 61, 61, 62, 63, 61, 59, 57, 56, 56, 57, 58, 59, 63, 61, 59, 56, 57, 59, 60, 62 };
        U16 durations[] = { 136, 136, 272, 272, 272, 272, 272, 136, 136, 136, 272, 136, 136, 272, 136, 136, 136, 136, 272, 136, 272, 272, 272, 136, 136, 136, 136, 136, 136, 136, 136, 136, 136, 136, 136, 136, 136, 136, 136 };
        
        for (U32 i = 0; i < sizeof(tones) / 2; ++i) {
            for (U8 v = 0; v < 2; v++) {
                BeepSPC(tones[i], durations[i] / 2);
                TSleep(10);
            }
            if (i % 4 == 0) BeepSPC(tones[i] - 12, durations[i] / 2);
            TSleep(1000/70);
            KDogWatchPTick(0);
            if (KBState.keys['\x1b']) {
                break;
            }
        }
    }
    else if (!StrCmp(cmd, "mus")) {
        TaskNew((U32)CMus, 0x10, 0x08);
    }
    else {
        TTYUPrint("Unk ");
        TTYUPrint(cmd);
    }
}

U0 mainloop() {
    Char buffer[50] = {0};
    KDogWatchPEnd(0);
    for (;;) {
        TTYUPrint("$!A\\$ $!F");
        KBRead(buffer, 50);
        // KDogWatchPPlay(0);
        termrun(buffer);
        TTYUPrintC('\n');
        MemSet(buffer, 0, 50);
    }
}
U0 programtest()
{
    // BsfApp bapp = BsfFromBytes((Ptr)0x8400);
    // UserSegment = (Segment) {
    //     .addr = bapp.data,
    //     .length = bapp.header.CodeS,
    // };
    // BsfExec(&bapp);
    // BsfApp bapp = BsfFromBytes("BOSY&\0\0\0\xf4\xb8\xfa\xfa\xbf\xde\xb0\x01\xbe\x11\0\0\0\xcd\x80\xeb\xfe$!AHello, world!$!F\n\0");
    // BsfExec(&bapp);
}
