#include <misc/time.h>

U32 MSleep(U32 millis) {
    U32 s = PITTime;
    U32 e = PITTime + millis;

    while (PITTime < e) asmv("pause");
    return PITTime - s;
}