#include <drivers/tty.h>


U0 TTYRenderS() {
    for (Char c;PTermRead(VTerm, 1, &c, 1);) {
        if (c == ASCIIPCtrl) {
            SerialWrite('\x1b');SerialWrite('[');SerialWrite('2');SerialWrite('J');
            SerialWrite('\x1b');SerialWrite('[');SerialWrite('H');
        }
        SerialWrite(c);
    }
}