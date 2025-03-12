#pragma once
#include <lib/Types.h>

U16 IPChecksum(U16 *buf, U32 len);
U16 HtonW(U16 w);
U32 HtonD(U32 w);

typedef struct {
    U16 src_port;
    U16 dst_port;
    U16 length;
    U16 checksum;
} __attribute__((packed)) UDPHeader;

U0 UDPSend(U16 lp, U16 rp, U32 lip, U32 rip, Ptr payload, U32 count);