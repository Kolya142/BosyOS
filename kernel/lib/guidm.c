#include "guidm.h"

GUIDM GUIDMUnicue() {
    GUIDM guid;
    guid.dd = 0x1234 ^ (unsigned short)(calculate_shash(SysClock));
    guid.dm = 0x5678 ^ (unsigned short)(calculate_shash(SysClock));
    guid.de = 0x9abc ^ (unsigned short)(calculate_shash(SysClock));
    guid.df = 0xdef0 ^ (unsigned short)(calculate_shash(SysClock));

    return guid;
}