// Kernel
#include <kernel/KDogWatch.h>
#include <kernel/KTasks.h>
#include <kernel/KPanic.h>
#include <kernel/KShell.h>
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

// extern U0 KernelDebug();
U0 mainloop();
U0 cmdloop();
U0 loop();

U0 KernelMain() {
    HeapInit();
    TTYInit();
    // TTYSwitch(TTYC_VGA);
    VgaInit();
    TTYClear();
    TTYSwitch(3);
    // TTerm.width = 80;
    // TTerm.height = 25;
    TTYCursor = 0;
    KDogWatchInit();
    GDTInit();
    IDTInit();
    PICMap();
    PITInit();
    TaskInit();
    KDogWatchLog("Switching to 320x200", False);
    VgaGraphicsSet();
    // VESAInit();
    TTYCursor = 0;
    KDogWatchLog("System initializing start", False);

    U16 mem = MemorySize();
    PrintF("$!EDetected Memory size $!C- $!B0x%2xKB$!F\n\n", mem);
    if (mem < 64000) {
        KDogWatchLog("$!CWarning: Memory size < $!B64MB$!F", False);
    }

    VFSInit();
    KDogWatchLog("Initialized \"vfs\"", False);

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
    // PCIInit();
    // PCIDevicesCheck();
    // KDogWatchLog("Initialized \"PCI\"", False);
    // USBInit();
    // KDogWatchLog("Initialized \"USB\"", False);
    // SerialInit();
    // KDogWatchLog("Initialized \x9Bserial\x9C", False);
    // // RTL8139Init();
    // // KDogWatchLog("Initialized \"rtl8139\"", False);
    PS2Init();
    KDogWatchLog("Initialized \"ps/2\"", False);
    KBInit();
    KDogWatchLog("Initialized \x9Bkeyboard\x9C", False);
    MouseInit(); // Portal to hell
    KDogWatchLog("Initialized \"mouse\"", False);
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
    // VFSMount("tmp/", (Ptr)RFSReadV, (Ptr)RFSWriteV, (Ptr)RFSReadDirV);
    KDogWatchLog("Initialized \x9Bramfs\x9C", False);

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
    // TTYClear();
    // TTYCursor = 0;

    TTYSwitch(0);

    Sleep(500);
    TaskNew((U32)mainloop, 0x10, 0x08);
    // mainloop();
    // TaskNew((U32)mainloop);
    // TaskNew((U32)loop);
    // TaskingIs = True;
    for(;;);
    CpuHalt();

}
// INT_DEF(KernelDebug) {
//     U32 c = TTYCursor;
//     // TTYSwitch(TTYC_SER);
//     TTYCursor = 0;
//     PrintF("EIP: %08x ESP: %08x EBP: %08x\n", regs->eip, regs->useresp, regs->ebp);
//     PrintF("EAX: %08x EBX: %08x ECX: %08x EDX: %08x\n", regs->eax, regs->ebx, regs->ecx, regs->edx);
//     // TTYSwitch(TTYC_RES);
//     TTYCursor = c;
// }

extern Bool VRMState;

static U0 TimeUpd(Ptr this) {
    Win *win = this;
    WPrintF(win, 0, 0, "%d:%d:%d    ", SystemTime.hour, SystemTime.minute, SystemTime.second);
}

U0 cmdloop() {
    for (;;) {
        Sleep(1000/60);
    }
}

U0 mainloop() {
    TTYClear();
    PrintF("First time in bosyos?\n$!BUse$!F command \"$!7tut$!F\" to learn basic things\n");
    Win time;
    time = WinMake(320 - 10 - 8*6, 10, 8*6, 6, "Time", WIN_UNMOVEBLE);
    time.update = TimeUpd;
    Char buffer[50] = {0};
    // TaskNew((U32)cmdloop, 0x10, 0x08);
    
    WinSpawn(&time);
    TTYSwitch(3);
    TTYUPrint("$!7(BosyOS) $!F");
    TTYSwitch(0);
    // TaskNew((U32)cmdloop, 0x10, 0x08);
    TTYUPrint("$!A\\$ $!F");
    for (;;) {
        TTerm.render();
        if (VTerm->in.count) {
            KBRead(buffer, 50);
            if (TTermID == 3) {
                if (!StrCmp(buffer, "panic")) {
                    KPanic("Test panic", True);
                }
                else if (!StrCmp(buffer, "drivers")) {
                    for (U32 i = 0; i < 50; ++i) {
                        if (Drivers[i].name) {
                            PrintF("$!Adriver$!F: $!7%s$!F | ", Drivers[i].name);
                            U32 x = (TTerm.width - 32);
                            TTYCursor = x + (TTYCursor / TTerm.width) * TTerm.width;
                            PrintF("$!B%X$!F-$!B%X$!F\n", Drivers[i].d1, Drivers[i].d2);
                        }
                    }
                }
                TTYUPrint("\n$!7(BosyOS) $!F");
            }
            else {
                termrun(buffer);
                TTYUPrint("\n$!A\\$ $!F");
            }
            MemSet(buffer, 0, 50);
        }
        TTerm.render();
        Sleep(16);
    }
}