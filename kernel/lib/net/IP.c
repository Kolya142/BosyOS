#include <lib/net/IP.h>

U16 IPChecksum(U16 *buf, U32 len) {
    U32 sum = 0;
    for (; len > 1; len -= 2) sum += *buf++;
    if (len == 1) sum += *(U8*)buf;
    while (sum >> 16) sum = (sum & 0xFFFF) + (sum >> 16);
    return ~sum;
}

U16 HtonW(U16 w) {
    return (w >> 8) | ((w & 0xff) << 8);
}
U32 HtonD(U32 w) {
    return (HtonW(w >> 16)) | (HtonW(w & 0xffff) << 16);
}