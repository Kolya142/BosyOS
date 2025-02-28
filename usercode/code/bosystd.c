#include <stdint.h>
#include "bosystd.h"
uint32_t drvcall(uint32_t d1, uint32_t d2, uint32_t id, uint32_t value) {
    uint32_t v = value;
    syscall(0x13, d1, d2, id, (uint32_t)&v);
    return v;
}
uint32_t syscall(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e) { // x86 magic
    return _syscall(a, b, c, d, e);
}

float sin(float x) {
    float r;
    asm volatile ( "fldl %1\n"
        "fsin\n"
        "fstpl %0\n" : "=m"(r) : "m"(x) );
    return r;
}
float cos(float x) {
    float r;
    asm volatile ( "fldl %1\n"
        "fcos\n"
        "fstpl %0\n" : "=m"(r) : "m"(x) );
    return r;
}
float sqrt(float x) {
    float r;
    asm volatile ( "fldl %1\n"
        "fsqrt\n"
        "fstpl %0\n" : "=m"(r) : "m"(x) );
    return r;
}