#include <stdint.h>
#include "bosystd.h"
uint32_t drvcall(uint32_t d1, uint32_t d2, uint32_t id, uint32_t value) {
    uint32_t v = value;
    syscall(0x13, d1, d2, id, (uint32_t)&v);
    return v;
}
__attribute__((cdecl)) uint32_t syscall(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e) { // x86 magic
    volatile uint32_t ret;
    ret = _syscall(a, b, c, d, e);
    return ret;
}

void exit(uint32_t code) {
    syscall(0x15, 2, 0, code, 0);
}

float sin(float x) {
    float r;
    asm volatile ( "fld %1\n"
        "fsin\n"
        "fstp %0\n" : "=m"(r) : "m"(x) );
    return r;
}
float cos(float x) {
    float r;
    asm volatile ( "fld %1\n"
        "fcos\n"
        "fstp %0\n" : "=m"(r) : "m"(x) );
    return r;
}
float sqrt(float x) {
    float r;
    asm volatile ( "fld %1\n"
        "fsqrt\n"
        "fstp %0\n" : "=m"(r) : "m"(x) );
    return r;
}
float atan2(float y, float x) {
    float r;
    asm volatile (
        "fld %1\n"
        "fld %2\n"
        "fpatan\n"
        "fstp %0\n"
        : "=m"(r)
        : "m"(y), "m"(x) );
    return r;
}
float fmodf(float a, float b) {
    float r;
    asm volatile (
        "fld %1\n"
        "fld %2\n"
        "1:\n"
        "fprem\n"
        "fstsw %%ax\n"
        "sahf\n"
        "jp 1b\n"
        "fstp %%st(1)\n"
        "fstp %0\n"
        : "=m"(r)
        : "m"(a), "m"(b)
        : "ax" );
    return r;
}