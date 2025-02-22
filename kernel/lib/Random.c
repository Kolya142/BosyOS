#include <drivers/pit.h>
#include <lib/Random.h>
#include <lib/Time.h>

U32 RandomU() {
    static U32 seed = 0x1234FDEB;
    U32 pit = PITTime;
    U32 time = SystemTime.year;
    time |= SystemTime.day    << 3;
    time ^= SystemTime.hour   * 256;
    time |= SystemTime.minute * 20;
    time ^= SystemTime.second << 5;
    seed ^= (time * 230) | pit;
    seed *= 20432;
    seed ^= 0x239B2042 << (seed & 15);
    U32 y;
    __asm__ volatile (
        "movl $1, %%eax\n"
        "cpuid\n"
        "movl %%ecx, %0\n"
        : "=m"(y)
        :
        : "eax", "ebx", "ecx", "edx"
    );
    seed ^= y;
    seed ^= (seed >> 5) + (seed << 7);
    return seed;
}