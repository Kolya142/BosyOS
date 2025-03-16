#include <drivers/tty.h>
#include <drivers/serial/serial.h>


U0 TTYRenderS(TTY *this) {
    for (Char c;PTYRead(this->pty, 1, &c, 1);) {
        if (c == ASCIIPCtrl) {
            SerialWrite('\x1b');SerialWrite('[');SerialWrite('2');SerialWrite('J');
            SerialWrite('\x1b');SerialWrite('[');SerialWrite('H');
        }
        SerialWrite(c);
    }
}