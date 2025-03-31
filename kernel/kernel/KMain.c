// Kernel
#include <kernel/KDogWatch.h>
#include <kernel/KTasks.h>
#include <kernel/KPanic.h>
#include <kernel/KShell.h>
#include <kernel/KAlarm.h>
#include <kernel/KSys.h>
#include <kernel/KMem.h>

// Drivers
#include <drivers/input/keyboard.h>
#include <drivers/network/rtl8139.h>
#include <drivers/controllers/usb.h>
#include <drivers/controllers/ps2.h>
#include <drivers/serial/serial.h>
#include <drivers/input/mouse.h>
#include <drivers/misc/random.h>
#include <drivers/video/vesa.h>
#include <drivers/video/vga.h>
#include <drivers/disk/ide.h>
#include <drivers/time/pit.h>
#include <drivers/sys/beep.h>
#include <drivers/sys/pci.h>

// Miscellaneous
#include <misc/vfiles.h>
#include <misc/driverreg.h>
#include <misc/vdrivers.h>
#include <lib/sys/syscall.h>
#include <misc/wordgen.h>
#include <misc/multiboot.h>

// Lang
#include <lang/Tokenizer.h>
#include <lang/Compiler.h>

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
#include <fs/eifs/eifs.h>
#include <fs/iso9660.h>
#include <fs/ramfs.h>
#include <fs/romfs.h>
#include <fs/vfs.h>

// Arch/Cpu Functions
#include <arch/x86/sys/paging.h>
#include <arch/x86/sys/ring3.h>
#include <arch/x86/cpu/cpu.h>
#include <arch/x86/sys/gdt.h>
#include <arch/x86/cpu/fpu.h>
#include <arch/x86/sys/sys.h>
#include <arch/x86/cpu/pic.h>

// #define FASTBOOT

// extern U0 KernelDebug();
U0 mainloop();
U0 cmdloop();
U0 loop();

U0 KernelMain(struct MultiBoot *mbi) {
    mb = mbi;

    {
        Ptr ptr = (Ptr)((U32)mb)+8;
        for (;;) {
            struct MultiBootTag *tag = ptr;
            if (tag->type == 0) {
                break;
            }
            if (tag->type == 8) {
                struct {
                    U32 type;
                    U32 size;
                    uint64_t addr;
                    U32 pitch;
                    U32 width;
                    U32 height;
                    U8 bpp;
                    U8 fb_type;
                    U16 reserved;
                } __attribute__((packed)) *fb = (Ptr)tag;
                VVRM = (U32*)(U32)fb->addr;
                VVRM[100] = 0xFF00FF;
            }
            ptr += (tag->size + 7) & ~7;
        }
    }
    HeapInit();

    VFSInit();
    VFilesInit();
    KDogWatchLog("Initialized \"vfs\"", False);

    TTYInit();

    SerialInit();
    KDogWatchLog("Initialized \x9Bserial\x9C", False);
    CompilerInit();
    KDogWatchLog("Initialized \x9Bcompiler\x9C", False);

    U32 ptys = PTYNew(2048, 1, 1);
    U32 ptyg1 = PTYNew((WIDTH/8)*(HEIGHT/8)*5, WIDTH/8, HEIGHT/8);
    U32 ptyg2 = PTYNew((WIDTH/8)*(HEIGHT/8)*5, WIDTH/8, HEIGHT/8);
    U32 ptyg3 = PTYNew((WIDTH/8)*(HEIGHT/8)*5, WIDTH/8, HEIGHT/8);
    U32 ptyg4 = PTYNew((WIDTH/8)*(HEIGHT/8)*5, WIDTH/8, HEIGHT/8);
    TTYNew(TTYRenderS, ptys);
    TTYNew(TTYRenderG, ptyg1);
    TTYNew(TTYRenderG, ptyg2);
    TTYNew(TTYRenderG, ptyg3);
    TTYNew(TTYRenderG, ptyg4);
    TTYCurrent = 4;
    SerialPrintF("%p", VVRM);
    // TTYSwitch(TTYC_VGA);

    VgaInit();
    TTYSwitch(3);

    // TTYClear();
    // ((TTY*)TTYs.arr)[TTYCurrent].pty->width = 80;
    // ((TTY*)TTYs.arr)[TTYCurrent].pty->height = 25;
    ((TTY*)TTYs.arr)[TTYCurrent].pty->cursor = 0;
    GDTInit();
    IDTInit();
    PICMap();
    RTCUpdate();
    U32 t = SystemTime.second;
    while (SystemTime.second == t) {
        RTCUpdate();
    }
    PITInit();
    TaskInit();
    KDogWatchInit();
    // KDogWatchLog("Switching to 320x200", False);
    // VgaGraphicsSet();
    // VESAInit();
    ((TTY*)TTYs.arr)[TTYCurrent].pty->cursor = 0;
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

    KDogWatchLog("Initialized \x9Bpaging\x9C", False);

    KDogWatchLog("Setuping fpu", False);
    FPUBox();
    
    // Drivers
    KDogWatchLog("Setuping drivers", False);
    // PCIInit();
    // PCIDevicesCheck();
    // KDogWatchLog("Initialized \"PCI\"", False);
    // USBInit();
    // KDogWatchLog("Initialized \"USB\"", False);
    // // RTL8139Init();
    // // KDogWatchLog("Initialized \"rtl8139\"", False);
    PS2Init();
    KDogWatchLog("Initialized \"ps/2\"", False);
    #ifndef FASTBOOT
    MouseInit(); // Portal to hell
    KDogWatchLog("Initialized \"mouse\"", False);
    BeepInit();
    KDogWatchLog("Initialized \x9Bpc speaker\x9C", False);
    #endif
    IDEInit();
    KDogWatchLog("Initialized \x9Bide disk\x9C", False);
    VDriversReg();
    KDogWatchLog("Initialized \x9Bvdrivers\x9C", False);
    // Drivers end
    
    // FSs
    KDogWatchLog("Setuping FileSystems", False);

    ISO9660Init();
    KDogWatchLog("Initialized \x9BISO9660\x9C", False);

    RFSInit();
    VFSMount("tmp/", (Ptr)RFSReadV, (Ptr)RFSWriteV, Null);
    KDogWatchLog("Initialized \x9Bramfs\x9C", False);

    ISO9660DirEntry *initrom = ISO9660Get("initrom.");
    if (!initrom) {
        KPanic("Cannot get initrom.", False);
    }
    // return;
    ATARead(0, (Ptr)0x200000, initrom->extent_lba_le * 4, (initrom->data_length_le + 511) / 512);
    ROFSInit((Ptr)0x200000);
    KDogWatchLog("Initialized \x9Bromfs\x9C", False);
    RFSInit();
    KDogWatchLog("Initialized \x9Bramfs\x9C", False);

    // EIFInit();
    // KDogWatchLog("Initialized \"eifs\"", False);
    // DATInit();
    // KDogWatchLog("Initialized \"dat\"", False);
    // BOTFSInit();
    // KDogWatchLog("Initialized \"botfs\"", False);
    // UFSInit();
    // KDogWatchLog("Initialized \"ufs\"", False);

    KDogWatchLog("System Initialized", False);
    KDogWatchLog("Entering shell", False);
    TTYSwitch(1);
    TTYWrite(1, 1, "\x80", 1);
    KBInit();
    KDogWatchLog("Initialized \x9Bkeyboard\x9C", False);
    // ((TTY*)TTYs.arr)[TTYCurrent].pty->cursor = 0;

    // TaskNew((U32)mainloop, 0x10, 0x08);
    mainloop();
    // mainloop();
    // TaskNew((U32)mainloop);
    // TaskNew((U32)loop);
    // TaskingIs = True;
    for(;;);
    CpuHalt();

}
// INT_DEF(KernelDebug) {
//     U32 c = ((TTY*)TTYs.arr)[TTYCurrent].pty->cursor;
//     // TTYSwitch(TTYC_SER);
//     ((TTY*)TTYs.arr)[TTYCurrent].pty->cursor = 0;
//     PrintF("EIP: %08x ESP: %08x EBP: %08x\n", regs->eip, regs->useresp, regs->ebp);
//     PrintF("EAX: %08x EBX: %08x ECX: %08x EDX: %08x\n", regs->eax, regs->ebx, regs->ecx, regs->edx);
//     // TTYSwitch(TTYC_RES);
//     ((TTY*)TTYs.arr)[TTYCurrent].pty->cursor = c;
// }

extern Bool VRMState;

static U0 TimeUpd(Ptr this) {
    Win *win = this;
    WPrintF(win, 0, 0, "%d:%d:%d    ", SystemTime.hour, SystemTime.minute, SystemTime.second);
}

U0 lsfn(String name, VFSStat *stat) {
    PrintF("File: %s, Size: %d\n", name, stat->size);
}

static U0 ring3() {
    TaskingCan = True;
    for (;;);
}

U0 mainloop() {
    TTYSwitch(0);
    // Win win = WinMake(320 - 6 * 8 - 5, 5, 6 * 8, 6, "clock", WIN_UNMOVEBLE);
    // win.update = TimeUpd;
    // WinSpawn(&win);
    {
        VFSStat stat = {0};
        VFSLStat("test.bc", &stat);
        U8 *buf = MAlloc(stat.size);
        VFSRead("test.bc", buf, 0, stat.size);
        List vars = ListInit(sizeof(CompilerVariable));
        List compiled = Compiler(buf, vars);
        ListDestroy(&vars);
        if (compiled.count) {
            U32(*entry)() = compiled.arr;
            PrintF("Compiled\n");
            PrintF("\nRunning\n");
            U32 res = entry();
            PrintF("Result: %p\n", res);
            ListDestroy(&compiled);
        }
        MFree(buf);
    }

    // MFree(buf);

    Char inp[512];

    PrintF("$!A\\$ $!F");

    for (;;) {
        static const U32 days_in_months[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        U32 days = 0;

        for (U32 y = 2025; y < SystemTime.year; y++) {
            days += (y % 4 == 0) ? 366 : 365;
        }
        for (U32 m = 1; m < SystemTime.month; m++) {
            days += days_in_months[m - 1];
            if (m == 2 && (SystemTime.year % 4 == 0)) {
                days++;
            }
        }
        days += SystemTime.day - 1;
        String day_names[] = {
            "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
        };
        TTYFlush(TTYCurrent);
        U32 c = ((TTY*)TTYs.arr)[TTYCurrent].pty->cursor;
        ((TTY*)TTYs.arr)[TTYCurrent].pty->cursor = 0;
        PrintF("$*1%s %d:%d:%d.%d %c    $*0", day_names[(days + 2) % 7], SystemTime.hour, SystemTime.minute, SystemTime.second, PITTime % 1000, KBState.Key);
        ((TTY*)TTYs.arr)[TTYCurrent].pty->cursor = max(80, c);
        TTYFlush(TTYCurrent);

        if (TTYRead(TTYCurrent, 0, inp, 512)) {
            List vars = ListInit(sizeof(CompilerVariable));
            List compiled = Compiler(inp, vars);
            ListDestroy(&vars);
            if (compiled.count) {
                U32(*entry)() = compiled.arr;
                PrintF("Compiled\n");
                PrintF("\nRunning\n");
                U32 res = entry();
                PrintF("Result: %p\n", res);
                ListDestroy(&compiled);
            }
            PrintF("$!A\\$ $!F");
        }
        Sleep(10);
    }

    // VFSReadDir("/", lsfn);
    // PrintF("\n");
    // VFSReadDir("/etc", lsfn);

    // VFSStat stat = {0};
    // VFSLStat("/bin/init.elf", &stat);
    // U8 *buf = MAlloc(stat.size);
    // VFSRead("/bin/init.elf", buf, 0, stat.size);
    // SerialPrintF("Starting program %x %d\n", *(U32*)(buf), stat.size);

    // TTYCurrent = 1;
    // ELFLoad(buf);
    // // BsfExec(&app, 0, 1);
    // MFree(buf);

    // RingSwitch(ring3, (Ptr)0x3000);
}