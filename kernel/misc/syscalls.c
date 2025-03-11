// #include <kernel/KDogWatch.h>
// #include <drivers/keyboard.h>
// #include <drivers/serial.h>
// #include <misc/driverreg.h>
// #include <kernel/KPanic.h>
// #include <kernel/KTasks.h>
// #include <misc/syscall.h>
// #include <arch/x86/getreg.h>
// #include <misc/bsfexe.h>
// #include <lib/Random.h>
// #include <lib/MemLib.h>
// #include <arch/x86/beep.h>
// #include <arch/x86/gdt.h>
// #include <arch/x86/idt.h>
// #include <arch/x86/cpu.h>
// #include <arch/x86/sys.h>
// #include <lib/TTY.h>
// #include <arch/x86/io.h>

// static INTRegs states[256] = {0};
// static U32 statei = 3;

// U0 SCPrint(INTRegs *regs) {
//     U32 addr = regs->esi;
//     TTYUPrint((Ptr)addr);
// }
// U0 SCPrintH(INTRegs *regs) {
//     U32 data = regs->esi;
//     TTYUPrintHex(data);
// }
// U0 SCBeep(INTRegs *regs) {
//     asmv("sti");
//     U16 data = regs->esi;
//     Beep(data);
//     asmv("cli");
// }
// U0 SCBeepHz(INTRegs *regs) {
//     asmv("sti");
//     U16 data1 = regs->esi;
//     U16 data2 = regs->edi;
//     BeepHz(data1, data2);
//     asmv("cli");
// }
// U0 SCBeepSPC(INTRegs *regs) {
//     asmv("sti");
//     U16 data1 = regs->esi;
//     U16 data2 = regs->edi;
//     BeepSPC(data1, data2);
//     asmv("cli");
// }
// U0 SCSleep(INTRegs *regs) {
//     asmv("sti");
//     U16 data = regs->esi;
//     SleepM(data);
//     asmv("cli");
// }
// U0 SCKey(INTRegs *regs) {
//     regs->eax = KBState.Key;
// }
// U0 SCPutC(INTRegs *regs) {
//     U8 c = regs->esi;
//     TTYUPrintC(c);
// }
// U0 SCMAlloc(INTRegs *regs) {
//     regs->eax = (U32)MAlloc(regs->esi);
// }
// U0 SCFree(INTRegs *regs) {
//     MFree((Ptr)regs->esi);
// }
// U0 SCTime(INTRegs *regs) {
//     U32 time;
//     DriverCall(0x09ca4b4d, 0x5fa68611, 1, &time);
//     regs->eax = time;
// }
// U0 SCTSet(INTRegs *regs) {
//     U32 c = TTYCursor;
//     // TTYCursor = regs->esi + regs->edi * TTerm.width;
//     // TTYPuter(regs->ebx);
//     // TTYCursor = c;
// }
// U0 SCTGet(INTRegs *regs) {
//     regs->eax = vga[regs->esi + regs->edi * TTerm.width] & 0xff;
// }
// U0 SCRandomU(INTRegs *regs) {
//     regs->eax = RandomU();
// }
// U0 SCUpTime(INTRegs *regs) {
//     U32 time;
//     DriverCall(0x09ca4b4d, 0x5fa68611, 0, &time);
//     regs->eax = time;
// }
// U0 SCKeysGet(INTRegs *regs) {
//     U8 *buf = (U8*)regs->esi;
//     if (!buf)
//         return;
//     MemSet(buf, 0, 32);
//     for (U32 i = 0; i < 32; ++i) {
//         for (U32 j = 0; j < 8; ++j) {
//             if (KBState.keys[i*8+j])
//                 buf[i] |= (1 << j);
//         }
//     }
// }
// U0 SCReboot() {
//     PowerReboot();
// }
// U0 SCPowerOff() {
//     PowerOff();
// }
// // ERROR: This function breaks registers
// U0 SCDriverACC(INTRegs *regs) {
//     asmv("sti");
//     DriverCall(regs->esi, regs->edi, regs->ebx, (Ptr)regs->edx);
//     asmv("cli");
// }
// U0 SCSaveMe(INTRegs3 *regs) {
//     if (regs->esi > 255) {
//         regs->eax = -1;
//         return;
//     }
//     MemCpy(&states[regs->esi], regs, sizeof(INTRegs));
//     ++statei;
//     regs->eax = False;
// }
// U0 SCLoadMe(INTRegs3 *regs) {
//     if (regs->esi > 255) {
//         regs->eax = -1;
//         return;
//     }
//     U32 old_ebx = regs->ebx;
//     --statei;

//     MemCpy(regs, &states[regs->esi], sizeof(INTRegs));
    
//     regs->eax = True;

//     // regs->ss = 0x10;
//     // regs->esp = TSS.esp0;

//     // TTYSwitch(TTYC_SER);
//     // for (U32 i = 0; i < sizeof(INTRegs)/sizeof(U32); ++i) {
//     //     PrintF("%x", ((U32*)regs)[i]);
//     // }
//     // TTYSwitch(TTYC_RES);
// }

// U0 SCRun(INTRegs *regs) {
//     Ptr data = (Ptr)0x5000;
//     BsfApp app = BsfFromBytes(data);
//     BsfExec(&app, 0, regs->esi);
// }
// U0 SCYield(INTRegs *regs) {
//     MemCpy(&TaskTail->regs, regs, sizeof(INTRegs));
//     if (regs->ebx) return;
//     TaskNext();
//     MemCpy(regs, &TaskTail->regs, sizeof(INTRegs));
//     regs->ebx = 1;
// }

void SysCallSetup() {
    // SysCallSet(SCPrint, 1);
    // SysCallSet(SCPrintH, 2);
    // SysCallSet(SCBeep, 3);
    // SysCallSet(SCSleep, 4);
    // SysCallSet(SCKey, 5);
    // SysCallSet(SCPutC, 6);
    // SysCallSet(SCBeepHz, 7);
    // SysCallSet(SCBeepSPC, 8);

    // SysCallSet(SCMAlloc, 9);
    // SysCallSet(SCFree, 0xA);
    // SysCallSet(SCTime, 0xB);
    // SysCallSet(SCTSet, 0xC);
    // SysCallSet(SCTGet, 0xD);
    // SysCallSet(SCRandomU, 0xE);
    // SysCallSet(SCUpTime, 0xF);
    // SysCallSet(SCKeysGet, 0x10);
    // SysCallSet(SCReboot, 0x11);
    // SysCallSet(SCPowerOff, 0x12);
    // SysCallSet(SCDriverACC, 0x13);
    // SysCallSet(SCSaveMe, 0x14);
    // SysCallSet(SCLoadMe, 0x15);
    // SysCallSet(SCRun, 0x16);
    // SysCallSet(SCYield, 0x17);
}