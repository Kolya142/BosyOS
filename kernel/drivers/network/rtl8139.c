#include <kernel/KDogWatch.h>
#include <drivers/network/rtl8139.h>
#include <misc/driverreg.h>
#include <lib/memory/MemLib.h>
#include <arch/x86/cpu/io.h>

// // realization in rtl8139.s // FIXME
// extern U0 RTL8139Start();
// static Ptr RTL8139RXBuffer = Null;

// Bool RTL8139Read(NetPackage *pck) {
//     static U32 rxt = 0;

//     if (!RTL8139RXBuffer) {
//         return False;
//     }

//     U16 rxs = RTL8139_READ16(rxt);
//     U16 rxl = RTL8139_READ16(rxt + 2);

//     if (!(rxs & 1)) {
//         return False;
//     }
//     if (rxl > 2048) {
//         return False;
//     }

//     U8 *buf = (U8*)(RTL8139RXBuffer + rxt + 4);
//     MemCpy(pck->data, buf, rxl);
//     rxt = (rxt + rxl + 4 + 3) & ~3;
//     RTL8139_WRITE16(0x38, rxt);
//     return True;
// }
// NetMac RTL8139MacGet() {
//     NetMac mac;
//     mac.w1 = RTL8139_READ8(0) | (RTL8139_READ8(1) << 8);
//     mac.w2 = RTL8139_READ8(2) | (RTL8139_READ8(3) << 8);
//     mac.w3 = RTL8139_READ8(4) | (RTL8139_READ8(5) << 8);
//     return mac;
// }

// U0 RTL8139Write(NetPackage *pck) {
//     static U32 txs = 0;

//     U32 txa = RTL8139_READ32(0x20 + txs * 4);
//     U32 pcks = sizeof(NetPackageEthernet) + pck->length;
//     pcks = max(60, pcks);

//     U8 *data = MAlloc(pcks);
//     MemSet(data, 0, pcks);

//     NetMac src_mac = RTL8139MacGet();

//     NetPackageEthernet *frame = (NetPackageEthernet*)data;
//     MemCpy(&frame->ip4, &pck->ip4, sizeof(NetPackageIP4));
//     MemCpy(&frame->src_mac, &src_mac, sizeof(NetMac));
//     MemCpy(&frame->dst_mac, &pck->dst, sizeof(NetMac));    
//     frame->ethertype = 0x0800;

//     MemCpy(data + sizeof(NetPackageEthernet), pck->data, pck->length);

//     MemCpy((Ptr)txa, data, pcks);

//     RTL8139_WRITE16(0x10 + txs * 4, pcks);

//     RTL8139_WRITE8(0x37, 4);

//     MFree(data);

//     txs = (txs + 1) % 4;
// }
// U32 RTL8139Status() {

// }

// // INT_REG(RTL8139Handler) {

// // }

// static U0 RTL8139DriverHandler(U32 id, U32 *value) {
//     switch (id)
//     {
//     case 0:
//         RTL8139Write((NetPackage*)value);
//         break;
//     case 1:
//         RTL8139Read((NetPackage*)value);
//         break;
//     case 2:
//         *((NetMac*)value) = RTL8139MacGet();
//         break;
//     case 3:
//         *value = RTL8139Status();
//         break;
//     default:
//         *value = 0;
//         break;
//     }
// }

U0 RTL8139Init() {
    // RTL8139Start();
    // if (!RTL8139IO)
    //     return;
    // RTL8139RXBuffer = MAlloc(8192 + 16);

    // RTL8139_WRITE32(0x30, (U32)RTL8139RXBuffer);

    // RTL8139_WRITE8(0x37, 0x10);
    // U32 timeout = 100000;
    // while (RTL8139_READ8(0x37) & 0x10 && --timeout);
    // if (!timeout) {
    //     KDogWatchLog("RTL8139 ERROR: reset timeout!\n", False);
    //     return;
    // }
    // DriverReg(0xbb149088, 0xc3442e1e, RTL8139DriverHandler, "rtl8139");
}