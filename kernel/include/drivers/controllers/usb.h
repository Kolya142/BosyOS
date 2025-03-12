#pragma once
#include <lib/Types.h>
#include <drivers/sys/pci.h>

#define USB_MAX_CONTROLLERS 8

extern PCIDevice *USBControllers[USB_MAX_CONTROLLERS];
extern U32 USBControllerCount;

U0 USBInit();