#include <lib/time/Time.h>

U32 Sleep(U32 millis) {
    U32 s = PITTime;
    U32 e = PITTime + millis;

    while (PITTime < e);
    return PITTime - s;
}