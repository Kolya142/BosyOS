#include <lib/Time.h>

U32 SleepM(U32 millis) {
    U32 s = PITTime;
    U32 e = PITTime + millis;

    while (PITTime < e);
    return PITTime - s;
}

SystemTimeS SystemTime;

U8 RTCRead(U8 reg) {
    POut(0x70, reg);
    return PIn(0x71);
}
U8 BCDToBin(U8 bcd) {
    return (bcd >> 4) * 10 + (bcd & 15);
}

U0 RTCUpdate() {
    SystemTime.year =   BCDToBin(RTCRead(9)) + 2000;
    SystemTime.month =  BCDToBin(RTCRead(8));
    SystemTime.day =    BCDToBin(RTCRead(7));
    SystemTime.hour =   BCDToBin(RTCRead(4));
    SystemTime.minute = BCDToBin(RTCRead(2));
    SystemTime.second = BCDToBin(RTCRead(0));
}