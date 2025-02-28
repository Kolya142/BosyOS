#include <arch/fpu.h>

U0 FPUBox() {
    U32 cr0;
    asmV (
        "fninit\n"
        "mov %%cr0, %0\n"
        "and $~4, %0\n"
        "or  $2, %0\n"
        "mov %0, %%cr0\n"
        : "=a"(cr0)
    );
}

// Absolute Portable Code

float sin(float x) {
    float r;
    asm volatile ( "flds %1\n"
        "fsin\n"
        "fstps %0\n" : "=m"(r) : "m"(x) );
    return r;
}
float cos(float x) {
    float r;
    asm volatile ( "flds %1\n"
        "fcos\n"
        "fstps %0\n" : "=m"(r) : "m"(x) );
    return r;
}
float sqrt(float x) {
    float r;
    asm volatile ( "flds %1\n"
        "fsqrt\n"
        "fstps %0\n" : "=m"(r) : "m"(x) );
    return r;
}
float pow(float x, float y) {
    float r;
    asm volatile ( 
        "flds %1\n"
        "fyl2x\n"
        "fmulp\n"
        "f2xm1\n"
        "fld1\n"
        "faddp\n"
        "fstps %0\n" : "=m"(r) : "m"(x), "m"(y) );
    return r;
}