#include <drivers/tty.h>
#include <drivers/serial/serial.h>
#include <lib/IO/TTY.h>


U0 TTYRenderS(TTY *this, U32 ttyid) {
    static const U8 fgs[] = {
        30, 34, 32, 36, 31, 35, 33, 90,
        37, 94, 92, 96, 91, 95, 93, 97
    };
    static const U8 bgs[] = {
        40, 44, 42, 46, 41, 45, 43, 100,
        47, 104, 102, 106, 101, 105, 103, 107
    };
    for (Char c;PTYRead(this->pty, 1, &c, 1);) {
        if (c == ASCIIPCtrl) {
            SerialPrintF("\x1b[2J\x1b[H");
        }
        else if (c == ASCIIPF1) {
            PTYRead(this->pty, 1, &c, 1);
            U8 color = c - ASCIIPCBlack;
            if (color < sizeof(fgs)) {
                SerialPrintF("\x1b[%dm", fgs[color]);
            }
        }
        else if (c == ASCIIPF2) {
            PTYRead(this->pty, 1, &c, 1);
            U8 color = c - ASCIIPCBlack;
            if (color < sizeof(bgs)) {
                SerialPrintF("\x1b[%dm", bgs[color]);
            }
        }
        else {
            SerialWrite(c);
        }
    }
}