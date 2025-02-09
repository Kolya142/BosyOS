#include "driverreg.h"

DriverEntry Drivers[256];
int DriverCount = 0;

void DriverAdd(GUIDM id, void *updater, U16 selector, U8 type_attr) {
    Drivers[DriverCount].DriverID = id;
    DriverCount++;
    IDTSetEntry(DriverCount, (U32)updater, selector, type_attr);
}