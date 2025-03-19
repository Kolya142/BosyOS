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
#include <drivers/serial/serial.h>
#include <drivers/input/mouse.h>
#include <drivers/misc/random.h>
#include <drivers/video/vesa.h>
#include <drivers/video/vga.h>
#include <drivers/disk/ide.h>
#include <drivers/time/pit.h>
#include <drivers/input/ps2.h>
#include <drivers/sys/beep.h>
#include <drivers/sys/pci.h>

// Miscellaneous
#include <misc/vfiles.h>
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

U0 KernelMain() {
    HeapInit();

    VFSInit();
    VFilesInit();
    KDogWatchLog("Initialized \"vfs\"", False);

    TTYInit();

    SerialInit();
    KDogWatchLog("Initialized \x9Bserial\x9C", False);

    U32 ptys = PTYNew(2048, 1, 1);
    U32 ptyg1 = PTYNew(2048, 320/6, 200/6);
    U32 ptyg2 = PTYNew(2048, 320/6, 200/6);
    U32 ptyg3 = PTYNew(2048, 320/6, 200/6);
    U32 ptyg4 = PTYNew(2048, 320/6, 200/6);
    TTYNew(TTYRenderS, ptys);
    TTYNew(TTYRenderG, ptyg1);
    TTYNew(TTYRenderG, ptyg2);
    TTYNew(TTYRenderG, ptyg3);
    TTYNew(TTYRenderG, ptyg4);
    TTYCurrent = 4;
    // TTYSwitch(TTYC_VGA);

    VgaInit();
    TTYSwitch(3);

    // TTYClear(); // FIXME
    // ((TTY*)TTYs.arr)[TTYCurrent].pty->width = 80;
    // ((TTY*)TTYs.arr)[TTYCurrent].pty->height = 25;
    ((TTY*)TTYs.arr)[TTYCurrent].pty->cursor = 0;
    KDogWatchInit();
    GDTInit();
    IDTInit();
    PICMap();
    PITInit();
    TaskInit();
    KDogWatchLog("Switching to 320x200", False);
    VgaGraphicsSet();
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
    KBInit();
    KDogWatchLog("Initialized \x9Bkeyboard\x9C", False);
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

    // RFSInit();
    // VFSMount("tmp/", (Ptr)RFSReadV, (Ptr)RFSWriteV, Null);
    // KDogWatchLog("Initialized \x9Bramfs\x9C", False);

    ATARead((Ptr)0x20000, 291, 64);
    ROFSInit((Ptr)0x20000);
    KDogWatchLog("Initialized \x9Bromfs\x9C", False);

    // MXInit();
    // KDogWatchLog("Initialized \"minix fs\"", False);
    // DATInit();
    // KDogWatchLog("Initialized \"dat\"", False);
    // BOTFSInit();
    // KDogWatchLog("Initialized \"botfs\"", False);
    // UFSInit();
    // KDogWatchLog("Initialized \"ufs\"", False);

    KDogWatchLog("System Initialized", False);
    KDogWatchLog("Entering shell", False);
    // // TTYClear(); // FIXME
    // ((TTY*)TTYs.arr)[TTYCurrent].pty->cursor = 0;

    // TaskNew((U32)mainloop, 0x10, 0x08);
    TTYSwitch(1);
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

U32 syscall(U32 id, U32 a, U32 b, U32 c, U32 d) {
    U32 ret;
    asmV (
        "int $0x80"
        : "=a"(ret)
        : "a"(id), "b"(a), "c"(b), "d"(c), "S"(d)
    );
    return ret;
}
U0 print(String text) {
    syscall(4, 1, (U32)text, StrLen(text), 0);
}

static U0 loop1() {
    Char buffer[50] = {0};
    for (;;) {
        MemSet(buffer, 0, 50);
        if (syscall(3, 0, (U32)buffer, 50, 0)) {
            if (!StrCmp(buffer, "panic")) {
                KPanic("panic command", True);
            }
            else if (!StrCmp(buffer, "help")) {
                syscall(4, 1, (U32)"commands:\npanic\nhelp\n", 21, 0);
            }
        }
        Sleep(10);
    }
}

static U0 loop2() {
    Char buffer[50] = {0};
    print("$ ");
    for (;;) {
        MemSet(buffer, 0, 50);
        if (syscall(3, 0, (U32)buffer, 50, 0)) {
            termrun(buffer);
            print("$ ");
        }
        Sleep(10);
    }
}

U0 ring3() {
    TTYCurrent = 1;
    asmV(
        "int $0x80"
        :: "a"(11), "S"(loop2)
    );
    for(;;);
}

U0 lsfn(String name, VFSStat *stat) {
    PrintF("File: %s\n", name);
}

U0 mainloop() {
    for (U32 t = 0; t < TTYs.count; ++t) {
        TTYCurrent = t;
        PrintF("Terminal %d\n", t);
    }
    TTYSwitch(0);
    // Win win = WinMake(320 - 6 * 8 - 5, 5, 6 * 8, 6, "clock", WIN_UNMOVEBLE);
    // win.update = TimeUpd;
    // WinSpawn(&win);

    {
        // VFSReadDir("/", lsfn);
        U8 buf[16] = {0};
        U32 readed = VFSRead("/dev/urandom", buf, 0, 16);
        
        PrintF("Readed %d entropy bytes: ", readed);
        for (U32 i = 0; i < readed; ++i) {
            PrintF("%1X ", buf[i]);
        }
        PrintF("\n");
    }

    VFSReadDir("/", lsfn);

    VFSStat stat;
    VFSLStat("test.bsf", &stat);
    U8 *buf = MAlloc(stat.size);
    VFSRead("test.bsf", buf, 0, stat.size);
    BsfApp app = BsfFromBytes(buf);
    PrintF("Starting program\n");
    BsfExec(&app, 0, 1);
    MFree(buf);
    PrintF("Failed to run program\n");
    // RingSwitch(ring3, (Ptr)0x300000);
}