#include <lib/guidm.h>
#include <arch/idt.h>
#pragma once
typedef struct DriverEntry {
    GUIDM DriverID;
} DriverEntry;

extern DriverEntry Drivers[256];
extern int DriverCount;

void DriverAdd(GUIDM id, void *updater, U16 selector, U8 type_attr);