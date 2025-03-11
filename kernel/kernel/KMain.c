// Kernel
#include <kernel/KDogWatch.h>
#include <kernel/KTasks.h>
#include <kernel/KShell.h>
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
#include <drivers/ps2.h>

// Miscellaneous
#include <misc/driverreg.h>
#include <misc/vdrivers.h>
#include <misc/syscall.h>
#include <misc/wordgen.h>
#include <misc/bsfexe.h>

// Lang
#include <lang/Tokenizer.h>

// KWS
#include <kws/win.h>

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
#include <fs/eifs.h>
#include <fs/vfs.h>

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
U0 backgroundloop();
// extern U0 KernelDebug();
U0 mainloop();
U0 loop();

U0 KernelMain() {
    HeapInit();
    TTYInit();
    // TTYSwitch(TTYC_VGA);
    VgaInit();
    TTYClear();
    TTerm.render = TTYRenderS;
    TTYCursor = 0;
    KDogWatchInit();
    GDTInit();
    IDTInit();
    PICMap();
    PITInit();
    KDogWatchLog("\x07Switching to 640x480", False);
    VgaGraphicsSet();
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
    VFSInit();
    KDogWatchLog("Initialized \"vfs\"", False);

    RFSInit();
    // VFSMount("tmp/", (Ptr)RFSReadV, (Ptr)RFSWriteV, (Ptr)RFSReadDirV);
    KDogWatchLog("Initialized \x9Bramfs\x9C", False);
    EIFInit();
    KDogWatchLog("Initialized \x9Beifs\x9C", False);

    // MXInit();
    // KDogWatchLog("Initialized \"minix fs\"", False);
    // DATInit();
    // KDogWatchLog("Initialized \"dat\"", False);
    // BOTFSInit();
    // KDogWatchLog("Initialized \"botfs\"", False);
    // UFSInit();
    // KDogWatchLog("Initialized \"ufs\"", False);

    KDogWatchLog("System Initialized", False);
    TTerm.render = TTYRenderG;
    KDogWatchLog("Entering shell\x07", False);
    // TTYClear();
    // TTYCursor = 0;

    SleepM(500);
    U32 tid1 = TaskNew((U32)backgroundloop, 0x10, 8);
    U32 tid2 = TaskNew((U32)mainloop, 0x10, 8);
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
U0 backgroundloop() {
    // for (;;) {

    // }
    TaskClose();
}