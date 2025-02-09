#include <drivers/kerboard.h>
#include <drivers/screen.h>
#include <drivers/mouse.h>
#include <lib/keyboardlib.h>
#include <lib/screenlib.h>
#include <lib/asciip.h>
#include <lib/string.h>
#include <lib/shash.h>
#include <lib/acs.h>
#include <arch/sleep.h>
#include <arch/halt.h>
#include <arch/time.h>
// #include <arch/idt.h>
#include <fs/fs.h>
#include <kernel/windows.h>
#include <kernel/alloc.h>
#include <kernel/clock.h>
void TestWindowHandler(WindowData *this);
// No functions before KernelMain
void KernelMain() {
    HeapInit();
    TTYClearScreen();
    TTYPrint("BosyOS loaded!\n");
    TTYPrint("Skip loading interrupts\n");
    {
        // IDTInit();
        // PITInit();   
        // outb(0x20, 0x11);
        // outb(0xA0, 0x11);
        // outb(0x21, 0x20);
        // outb(0xA1, 0x28);
        // outb(0x21, 0x04);
        // outb(0xA1, 0x02);
        // outb(0x21, 0x01);
        // outb(0xA1, 0x01);
        // outb(0x21, 0x00);
        // outb(0xA1, 0x00);
        // outb(0x21, 0xFD);

        // asm("sti");
    }
    TTYPrint("Loading SykoFS\n");
    FileSystem *fs = HeapAlloc(sizeof(FileSystem));
    TTYPrint("Testing SykoFS\n");
    {
        File *testfile = FCreate(fs, "test.txt");
        FWrite(fs, testfile, "T", 1);
        testfile->i = 0;
        char c;
        FRead(fs, testfile, &c, 1);
        if (c != 'T') {
            TTYPrint("SykoFS or AbstractFileSystem works incorrect\n");
            Halt();
        }
        else {
            TTYPrint("SykoFS and AbstractFileSystem works correct\n");
        }
    }
    // TTYPrint("Loading keyboard driver\n");
    // KeyboardInit();
    // TTYPrint("Keyboard Guid:");
    // PrintHex(Drivers[0].DriverID.dd << 16 | Drivers[0].DriverID.dm);
    // TTYPrintChar('-');
    // PrintHex(Drivers[0].DriverID.de << 16 | Drivers[0].DriverID.df);
    // TTYPrint("\nLoading mouse driver\n");
    // MouseInit();
    // WindowData *mywindow = WindowNew(TestWindowHandler);
    // mywindow->x = 0;
    // mywindow->y = 0;
    // mywindow->w = 35;
    // mywindow->h = 23;
    // MemCpy(mywindow->title, "BosyShell ", 10);
    // mywindow->title_len = 10;
    BufferFirstToSecond();
    TTYClearScreen();
    usleep(5000);
    char shell_state = 0;
    int shell_cursor = 80;
    char shell_buffer[80*25];
    char lastkey = 128;
    TTYPrintPos("BosyShell, press esc to switch modes", 0, 0);
    MemCpy(shell_buffer, vga, 4000);
    TTYClearScreen();
    for (;;) {
        // { // Real Screen Clear
        //     MemSet(vga, 0, 4000);
        //     MemSet(vvga, 0, 4000);
        // } 
         
        { // Screen clear
            BufferSecondToFirst();
        }
        // else { // Update shell
        //     MemCpy(vvga, shell_buffer, 4000);int c = TTYCursor;TTYCursor = shell_cursor;

        //     TTYPrintPos("3rf", 0, 1);

        //     shell_cursor = TTYCursor;TTYCursor = c;MemCpy(shell_buffer, vvga, 4000);
        // }
        { // Update esc
            if (!(KeyboardCode & 128) && lastkey) {
                if (KeyboardKey == '\x1b') {
                    shell_state ^= 1;
                }
            }
            lastkey = KeyboardCode & 128;
        }
        { // Update clock
            SysTick();
            U8 c = TTYCurrentColor;
            TTYSetColor(White, Black);
            CursorPushMove(80 - 22, 0);
            PrintHex(SysClock);
            CursorPop();
            TTYCurrentColor = c;
        }
        { // Update keyboard
            KeyboardAsk();
            U8 scode = KeyboardCode;
            U8 key = KeyboardScanCodeToKey(scode);
            WriteKeyboardKey(key, scode);
            KeyboardKey = key;
        }
        {
            BufferFlip();
        }
        usleep(500);
    }
}

// void TestWindowHandler(WindowData *this) {
//     static int cursorX = 0;
//     static int cursorY = 2;
//     static U8 PrevKey = 0;
//     static U8 buffer[35*23];
//     static U8 input[256];
//     static int inputi = 0;
//     static int bufferi = 0;
//     for (int x = this->x+1; x < this->x+this->w; x++) {
//         for (int y = this->y+1; y < this->y+this->h; y++) {
//             vvga[x + y * 80] = 0x0f20;
//         }
//     }
//     int c = TTYCursor;
//     TTYCursor = this->x + 1 + this->y * 80 + 80;

//     TTYPrint("Press ctrl+(left, right) to move\n");
//     TTYCursor += -(TTYCursor%80) + this->x + 1;
//     TTYPrint("window\n");
//     TTYCursor += -(TTYCursor%80) + this->x + 1;
//     int l = bufferi;
//     for (int i = 0; i < l; i++) {
//         if (((i != 0) && i % this->w == 0) || buffer[i] == '\n') {
//             TTYPrintChar('\n');
//             TTYCursor += this->x;
//             if (buffer[i] == '\n') 
//                 continue;
//         }
//         TTYPrintChar(buffer[i]);
//     }
//     TTYPrint("$ ");
//     l = StrLen(input);
//     for (int i = 0; i < l; i++) {
//         if (((i != 0) && ((i+3) % this->w) == 0) || input[i] == '\n') {
//             TTYPrintChar('\n');
//             TTYCursor += this->x;
//             if (input[i] == '\n') 
//                 continue;
//         }
//         TTYPrintChar(input[i]);
//     }
//     TTYCursor = this->x + 1 + this->y * 80 + 80;
//     TTYCursor += cursorX + cursorY * 80;

//     if (!(KeyboardCode & 128) && PrevKey) {
//         if (KeyboardKey == '\r') {
//             inputi = 0;
//             buffer[bufferi] = '$';
//             bufferi++;
//             buffer[bufferi] = ' ';
//             bufferi++;
//             int inputn = StrLen(input);
//             MemCpy(buffer+bufferi, input, inputn);
//             bufferi += inputn;
//             buffer[bufferi++] = '\n';
//             bufferi++;
            
//             {
//                 if (!StrCmp(input, "rand")) {
//                     buffer[bufferi++] = '0';
//                     buffer[bufferi++] = 'x';
//                     U32 rand = calculate_shash(SysClock);
//                     U8 *bytes = (U8*)&rand;
//                     buffer[bufferi++] = hex[bytes[3] >> 4];
//                     buffer[bufferi++] = hex[bytes[3] & 0xf];
//                     buffer[bufferi++] = hex[bytes[2] >> 4];
//                     buffer[bufferi++] = hex[bytes[2] & 0xf];
//                     buffer[bufferi++] = hex[bytes[1] >> 4];
//                     buffer[bufferi++] = hex[bytes[1] & 0xf];
//                     buffer[bufferi++] = hex[bytes[0] >> 4];
//                     buffer[bufferi++] = hex[bytes[0] & 0xf];
//                 }
//             }
//             buffer[bufferi++] = '\n';
//             bufferi++;  

//             MemSet(input, 0, 256);
//         }
//         else if (KeyboardKey == BACKSPACE) {
//             if (inputi >= 0) {
//                 input[inputi--] = ' ';
//             }
//         }
//         else if (KeyboardKey <= 0x7f) {
//             input[inputi] = KeyboardKey;
//             inputi++;
//         }
//     }

//     TTYCursorMove();

//     TTYCursor = c;
    
//     if (KeyboardCtrl && !(KeyboardCode & 128) && PrevKey) {
//         if (KeyboardKey == KEY_RIGHT && this->x+this->w < 78) {
//             this->x++;
//         }
//         if (KeyboardKey == KEY_LEFT && this->x > 0) {
//             this->x--;
//         }
//     }
//     PrevKey = KeyboardCode & 128;
// }