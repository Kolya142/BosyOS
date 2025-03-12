#include <drivers/controllers/usb.h>
#include <lib/IO/TTY.h>

PCIDevice *USBControllers[USB_MAX_CONTROLLERS];
U32 USBControllerCount = 0;

U0 USBInit() {
    for (U32 i = 0; i < 256 * 32 * 8; ++i) {
        PCIDevice *dev = &PCIDevices[i];
        if (dev->vendorid == 0xFFFF) continue;
        if (dev->classcode == 0x0C && dev->subclass == 0x03) {
            SerialPrintF("USB UHCI found at %2X:%2X:%2X", i / (32 * 8), (i / 8) % 32, i % 8);
            USBControllers[USBControllerCount++] = dev;
            break;
        }
    }
}