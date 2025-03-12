#include <drivers/sys/pci.h>
#include <lib/IO/TTY.h>

PCIDevice *PCIDevices;
extern String PCIClasses[] = {
    "Unclassified",    // 0
    "Drive",    // 1
    "Network Adapter",    // 2
    "Display Controller",    // 3
    "Multimedia Controller",    // 4
    "Memory Controller",    // 5
    "Bridge",    // 6
    "Simple Communication Controller",    // 7
    "Base System Peripheral",    // 8
    "Input Device Controller",    // 9
    "Docking Station",    // A
    "Processor",    // B
    "Serial Bus Controller",    // C
    "Wireless",    // D
    "Intelligent Controller",    // E
    "Satellite Communication Controller",    // D
    "Encryption Controller",    // F
    "Processing Accelator",    // 10
    "Non-Essential Instrumentation",    // 11
    "",    // 12
    "Co-Processor",    // 13
    "",    // 14
    ""    // 15
};

U0 PCIInit() {
    PCIDevices = MAlloc(sizeof(PCIDevice) * 32 * 256 * 8);
}

U16 PCIConfigReadWord(U8 bus, U8 slot, U8 func, U8 offset) {
    U32 addr = (U32)((bus << 16) | (slot << 11) | (func << 8) | (offset & 0xFC) | 0x80000000);
    POut32(0x0CF8, addr);
    PIn32(0x0CF8);
    return (U16)(PIn32(0x0CFC) >> ((offset & 3) * 8));
}
U32 PCIConfigReadDWord(U8 bus, U8 slot, U8 func, U8 offset) {
    U32 addr = (U32)((bus << 16) | (slot << 11) | (func << 8) | (offset & 0xFC) | 0x80000000);
    POut32(0x0CF8, addr);
    PIn32(0x0CF8);
    return PIn32(0x0CFC);
}

U16 PCIVendorCheck(U8 bus, U8 slot) {
    return PCIConfigReadWord(bus, slot, 0, 0);
}

U0 PCIDeviceCheck(U8 bus, U8 slot, U8 func, PCIDevice *dev) {
    dev->vendorid = PCIConfigReadWord(bus, slot, func, 0);
    if (dev->vendorid == 0xFFFF) {
        return;
    }

    dev->deviceid = PCIConfigReadWord(bus, slot, func, 2);
    dev->classcode = PCIConfigReadWord(bus, slot, func, 8);
    dev->subclass = PCIConfigReadWord(bus, slot, func, 9);
    dev->prog_if = PCIConfigReadWord(bus, slot, func, 10);

    PCIBARsGet(bus, slot, func, dev);
    // PrintF("PCI Device Found: Bus %X, Slot %X, Func %X, Vendor %X, Device ID %X, Class %X, Subclass %X\n",
    //              bus, slot, func, dev->vendorid, dev->deviceid, dev->classcode, dev->subclass);
}

U0 PCIDevicesCheck() {
    for (U16 bus = 0; bus < 256; ++bus) {
        for (U8 slot = 0; slot < 32; ++slot) {
            U16 vendor = PCIVendorCheck(bus, slot);
            if (vendor == 0xFFFF) continue;

            U8 headerType = (PCIConfigReadDWord(bus, slot, 0, 0x0C) >> 16) & 0xFF;
            U8 isMultifunction = headerType & 0x80;

            for (U8 func = 0; func < (isMultifunction ? 8 : 1); ++func) {
                U32 index = (bus * 32 * 8) + (slot * 8) + func;
                PCIDeviceCheck(bus, slot, func, &PCIDevices[index]);
            }
        }
    }
}

U0 PCIBARsGet(U8 bus, U8 slot, U8 func, PCIDevice *dev) {
    for (U32 i = 0; i < 6; ++i) {
        U32 bar = PCIConfigReadDWord(bus, slot, func, 0x10 + (i * 4));
        if (!bar) continue;

        if (bar & 1) {
            dev->bar[i] = bar & 0xFFFFFFFC;
        }
        else {
            dev->bar[i] = bar & 0xFFFFFFF0;
        }
    }
}