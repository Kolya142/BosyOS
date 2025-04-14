#include <drivers/disk/blkdev.h>
#include <lib/IO/TTY.h>

BlkDev Blks[16];

static U0 BDevATAWrite(struct BlkDev *blkdev, U32 start, Ptr buf, U32 size) {
    
}
static U0 BDevATARead(struct BlkDev *blkdev, U32 start, Ptr buf, U32 size) {

}

U0 BlkDevInit() {
    U32 blkid = 0;
    for (U16 bus = 0; bus < 256; ++bus) {
        for (U8 slot = 0; slot < 32; ++slot) {
            for (U8 func = 0; func < 8; ++func) {
                U32 index = (bus * 32 * 8) + (slot * 8) + func;
                PCIDevice *dev = &PCIDevices[index];
                if ((dev->classcode == 1 && dev->subclass == 1) || (dev->classcode == 0 && dev->subclass == 0x80)) {
                    // ATA Section
                    if (dev->bar[0] == 0) {dev->bar[0] = 0x1F0;}
                    if (dev->bar[1] == 0) {dev->bar[1] = 0x3F6;}
                    if (dev->bar[2] == 0) {dev->bar[2] = 0x170;}
                    if (dev->bar[3] == 0) {dev->bar[3] = 0x376;}
                    PrintF("ATA Bar: ");
                    SerialPrintF("ATA Bar: ");
                    for (U32 i = 0; i < 6; ++i) {
                        Blks[blkid].controller_data[i] = dev->bar[i]; 
                        PrintF("%p ", dev->bar[i]);
                        SerialPrintF("%p ", dev->bar[i]);
                    }
                    PrintF("\n");
                    SerialPrintF("\n");
                    Blks[blkid].exists = True;
                    Blks[blkid].id = blkid;
                    Blks[blkid].sector_size = 512;
                    Blks[blkid].write = NULL;
                    Blks[blkid].read = NULL;
                    ++blkid;
                }
            }
        }
    }
}