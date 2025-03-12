#pragma once
#include <arch/x86/cpu/io.h>
#include <lib/Types.h>

typedef struct PCIDevice {
    Char name[32];
    U16 vendorid;
    U16 deviceid;
    U8 classcode;
    U8 subclass;
    U8 prog_if;
    U8 header;
    U32 bar[6]; 
} PCIDevice;

extern PCIDevice *PCIDevices;
extern String PCIClasses[];

U0 PCIInit();

U16 PCIConfigReadWord(U8 bus, U8 slot, U8 func, U8 offset);
U32 PCIConfigReadDWord(U8 bus, U8 slot, U8 func, U8 offset);

U16 PCIVendorCheck(U8 bus, U8 slot);

U0 PCIDeviceCheck(U8 bus, U8 slot, U8 func, PCIDevice *dev);

U0 PCIDevicesCheck();

U0 PCIBARsGet(U8 bus, U8 slot, U8 func, PCIDevice *dev);