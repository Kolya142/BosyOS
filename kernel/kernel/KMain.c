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
#include <drivers/disk/blkdev.h>
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

U0 CompilerExtern(String code) {
    List vars = ListInit(sizeof(CompilerVariable));
    List compiled = Compiler(code, vars);
    ListDestroy(&vars);
    if (compiled.count) {
        PrintF("Result: %p\n", ((U32(*)())compiled.arr)());
        ListDestroy(&compiled);
    }
}
U0 CompilerStaticExtern(String code) {
    List vars = ListInit(sizeof(CompilerVariable));
    List compiled = Compiler(code, vars);
    ListDestroy(&vars);
    if (compiled.count) {
        for (U32 i = 0; i < compiled.count; ++i) {
            PrintF("%1X ", ((U8*)compiled.arr)[i]);
        }
        PrintF("\n");
        ListDestroy(&compiled);
    }
}


U0 KernelMain(struct MultiBoot *mbi) {
    mb = mbi;

    U32 mem = 0;

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
    KDogWatchLog("[INITIALIZED] \"Heap\"", False);
    KDogWatchLog("[INITIALIZED] \"VFS\"", False);

    TTYInit();
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
    // TTYSwitch(TTYC_VGA);
    KDogWatchLog("[INITIALIZING] \"serial\"", False);
    SerialInit();

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
    IDEInit();
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
    KDogWatchLog("System initializing start", False);
    // Drivers
    KDogWatchLog("Drivers section", False);
    KDogWatchLog("[INITALIZING] \"PCI\"", False);
    PCIInit();
    PCIDevicesCheck();
    KDogWatchLog("[INITIALIZED] \"PCI\"", False);
    KDogWatchLog("[INITIALIZING] ps/2", False);
    TTYFlush(TTYCurrent);
    PS2Init();
    KDogWatchLog("[INITIALIZED] ps/2", False);
    #ifndef FASTBOOT
    MouseInit();
    KDogWatchLog("[INITIALIZED] mouse", False);
    KBInit();
    KDogWatchLog("[INITIALIZED] keyboard", False);
    BeepInit();
    KDogWatchLog("[INITIALIZED] pc speaker", False);
    #endif
    BlkDevInit();
    KDogWatchLog("[INITIALIZED] blkdev", False);
    KDogWatchLog("[INITIALIZED] vdrivers", False);
    // Drivers end

    // KPanic("Test Panic", False);

    KDogWatchLog("[INITIALIZING] Compiler", False);
    CompilerInit();
    // VgaBlinkingSet(False);
    // VgaCursorDisable();
    // VgaCursorEnable();
    KDogWatchLog("[INITIALIZED] PIC", False);
    KDogWatchLog("[INITIALIZED] PIT", False);
    // KDogWatchLog("[INITIALIZED] VGA", False);
    // VgaGraphicsSet();
    // VRMClear(Purple);
    SysCallInit();
    SysCallSetup();
    
    KDogWatchLog("[INITIALIZED] SysCalls", False);

    
    // PagingInit();

    // KDogWatchLog("Initialized \x9Bpaging\x9C", False);

    // KDogWatchLog("Setuping fpu", False);
    // FPUBox();
    
    
    // FSs
    KDogWatchLog("FileSystems Section", False);

    ISO9660Init();
    KDogWatchLog("[INITIALIZED] \x9BISO9660\x9C", False);

    RFSInit();
    VFSMount("tmp/", (Ptr)RFSReadV, (Ptr)RFSWriteV, Null);
    KDogWatchLog("[INITIALIZED] \x9Bramfs\x9C", False);

    ISO9660DirEntry *initrom = ISO9660Get("initrom.");
    if (!initrom) {
        KPanic("[FAILED] initromfs doent exists", False);
    }
    // return;
    ATARead(0, (Ptr)0x200000, initrom->extent_lba_le * 4, (initrom->data_length_le + 511) / 512);
    ROFSInit((Ptr)0x200000);
    KDogWatchLog("[INITIALIZED] \x9Bromfs\x9C", False);
    RFSInit();
    KDogWatchLog("[INITIALIZED] \x9Bramfs\x9C", False);

    // EIFInit();
    // KDogWatchLog("Initialized \"eifs\"", False);

    // DATInit();
    // KDogWatchLog("Initialized \"dat\"", False);
    // BOTFSInit();
    // KDogWatchLog("Initialized \"botfs\"", False);
    // UFSInit();
    // KDogWatchLog("Initialized \"ufs\"", False);

    KDogWatchLog("System Initialized", False);
    TTYSwitch(0);
    TTYWrite(1, 1, "\x80", 1);
    PrintF("Running init\n");
    {
        VRMFlush();
        CompilerFunction func;
        MemSet(func.name, 0, 32);
        StrCpy(func.name, "Compiler");
        func.code.arr = CompilerExtern;
        ListAppend(&CompilerFunctions, &func);

        MemSet(func.name, 0, 32);
        StrCpy(func.name, "CompilerS");
        func.code.arr = CompilerStaticExtern;
        ListAppend(&CompilerFunctions, &func);

        MemSet(func.name, 0, 32);
        StrCpy(func.name, "Panic");
        func.code.arr = KPanic;
        ListAppend(&CompilerFunctions, &func);

        List vars = ListInit(sizeof(CompilerVariable));
        List compiled = Compiler("include \"/etc/start.ux\"", vars);
        ListDestroy(&vars);
        if (compiled.count) {
            U32 _ = ((U32(*)())compiled.arr)();
            ListDestroy(&compiled);
        }
    }
    // mainloop();
    // TaskNew((U32)mainloop);
    // TaskNew((U32)loop);
    // TaskingIs = True;
    for(;;);
    CpuHalt();

}
