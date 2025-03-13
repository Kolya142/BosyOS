#include <lib/IO/KeyboardLib.h>
#include <drivers/time/pit.h>
#include <lib/strings/String.h>
#include <lib/memory/MemLib.h>
#include <lib/IO/TTY.h>

U0 KBRead(String buf, U32 count) {
    while (!PTermRead(VTerm, 0, buf, count));
}