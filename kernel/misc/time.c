#include <misc/time.h>

U32 MSleep(U32 millis) {
    U32 s = PITTime;
    U32 e = PITTime + millis;

    while (PITTime < e) asmv("pause");
    return PITTime - s;
}

SystemTimeS SystemTime;

U8 RTCRead(U8 reg) {
    POut(0x70, reg);
    return PIn(0x71);
}

U0 RTCUpdate() {
    SystemTime.year = RTCRead(9) + 2000;
    SystemTime.month = RTCRead(8);
    SystemTime.day = RTCRead(7);
    SystemTime.hour = RTCRead(4);
    SystemTime.minute = RTCRead(2);
    SystemTime.second = RTCRead(0);
}