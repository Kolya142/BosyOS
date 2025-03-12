#pragma once
#include <misc/driverreg.h>
#include <lib/Types.h>

#define RTL8139_READ8(reg)  PIn(RTL8139IO + (reg))
#define RTL8139_WRITE8(reg, val) POut(RTL8139IO + (reg), val)
#define RTL8139_READ16(reg)  PIn16(RTL8139IO + (reg))
#define RTL8139_WRITE16(reg, val) POut16(RTL8139IO + (reg), val)
#define RTL8139_READ32(reg)  PIn32(RTL8139IO + (reg))
#define RTL8139_WRITE32(reg, val) POut32(RTL8139IO + (reg), val)

// realization in rtl8139.s
extern U32 RTL8139IO;
// realization in rtl8139.s
extern U32 RTL8139IRQ;


typedef struct NetPackageIP4 {
    U8 verIhl;
    U8 tos;
    U16 len;
    U16 id;
    U16 offset;
    U8 ttl;
    U8 protocol;
    U16 checksum;
    U32 srcip;
    U32 dstip;
} __attribute__((packed)) NetPackageIP4;

typedef volatile struct NetMac {
    U16 w1;
    U16 w2;
    U16 w3;
} __attribute__((packed)) NetMac;

typedef struct NetPackage {
    NetPackageIP4 ip4;
    NetMac dst;
    U32 length;
    U8 *data;
} NetPackage;

typedef struct NetPackageEthernet {
    NetMac dst_mac;
    NetMac src_mac;
    U16 ethertype;
    NetPackageIP4 ip4;
} __attribute__((packed)) NetPackageEthernet;

U0 RTL8139Init();

Bool RTL8139Read(NetPackage *pck);
NetMac RTL8139MacGet();
U0 RTL8139Write(NetPackage *pck);
U32 RTL8139Status();