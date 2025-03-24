#include <drivers/network/rtl8139.h>
#include <lib/net/IP.h>
// FIXME
// U16 IPChecksum(U16 *buf, U32 len) {
//     U32 sum = 0;
//     for (; len > 1; len -= 2) sum += *buf++;
//     if (len == 1) sum += *(U8*)buf;
//     while (sum >> 16) sum = (sum & 0xFFFF) + (sum >> 16);
//     return ~sum;
// }

U16 HtonW(U16 w) {
    return (w >> 8) | ((w & 0xff) << 8);
}
U32 HtonD(U32 w) {
    return (HtonW(w >> 16)) | (HtonW(w & 0xffff) << 16);
}

// U0 UDPSend(U16 lp, U16 rp, U32 lip, U32 rip, Ptr payload, U32 count) {
//     NetMac mac = RTL8139MacGet();
//     // PrintF("%C:%C:%C:%C:%C:%C\n", mac.w1 >> 8, mac.w1 & 0xFF, mac.w2 >> 8, mac.w2 & 0xFF, mac.w3 >> 8, mac.w3 & 0xFF);

//     NetPackage pck;

//     UDPHeader udp;
//     udp.src_port = HtonW(lp);
//     udp.dst_port = HtonW(rp);
//     udp.length = HtonW(sizeof(UDPHeader) + count);
//     udp.checksum = 0;

//     U8 *buffer = MAlloc(sizeof(UDPHeader) + count);
//     MemCpy(buffer, &udp, sizeof(UDPHeader));
//     MemCpy(buffer + sizeof(UDPHeader), payload, count);

//     pck.data = buffer;
//     pck.dst = (NetMac){0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
//     pck.length = sizeof(UDPHeader) + count;

//     pck.ip4.verIhl = 0x45;
//     pck.ip4.tos = 0;
//     pck.ip4.len = HtonW(sizeof(NetPackageIP4) + sizeof(UDPHeader) + count);
//     pck.ip4.id = HtonW(0x1234);
//     pck.ip4.offset = HtonW(0x4000);
//     pck.ip4.ttl = 64;
//     pck.ip4.protocol = 0x11;
//     pck.ip4.srcip = HtonD(lip);
//     pck.ip4.dstip = HtonD(rip);

//     pck.ip4.checksum = 0;
//     pck.ip4.checksum = IPChecksum((U16*)&pck.ip4, sizeof(NetPackageIP4));

//     RTL8139Write(&pck);
//     MFree(buffer);
// }