#include <stdint.h>
#include "bosystd.h"
#include "bosyos/drivers.h"
uint32_t drvcall(uint32_t d1, uint32_t d2, uint32_t id, uint32_t value) {
    uint32_t v = value;
    syscall(0x13, d1, d2, id, (uint32_t)&v);
    return v;
}
U0 _drvcall(uint32_t d1, uint32_t d2, uint32_t id, uint32_t *value) {
    syscall(0x13, d1, d2, id, (U32)value);
}
__attribute__((cdecl)) uint32_t syscall(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e) { // x86 magic
    volatile uint32_t ret;
    ret = _syscall(a, b, c, d, e);
    return ret;
}
U0 print(const String str) {
    syscall(1, (U32)str, 0, 0, 0);
}
U0 printhex(U32 num) {
    syscall(2, num, 0, 0, 0);
}
U0 beep(U16 dur) {
    syscall(3, dur, 0, 0, 0);
}
U0 sleep(U32 dur) {
    syscall(4, dur, 0, 0, 0);
}
U8 getkey() {
    return syscall(5, 0, 0, 0, 0);
}
U0 putc(Char c) {
    syscall(6, c, 0, 0, 0);
}
U0 beephz(U16 hz, U16 dur) {
    syscall(7, hz, dur, 0, 0);
}
U0 beepspc(U8 tone, U16 dur) {
    syscall(8, tone, dur, 0, 0);
}
Ptr malloc(U32 size) {
    return (Ptr)syscall(9, size, 0, 0, 0);
}
U0 free(Ptr ptr) {
    syscall(10, (U32)ptr, 0, 0, 0);
}
U32 time() {
    return syscall(11, 0, 0, 0, 0);
}
U0 tset(U16 x, U16 y, U8 c) {
    syscall(12, x, y, c, 0);
}
U8 tget(U16 x, U16 y) {
    return syscall(13, x, y, 0, 0);
}
U32 urandom() {
    return drvcall(DRAND, 0, 0);
}
U32 uptime() {
    return syscall(15, 0, 0, 0, 0);
}
U0 keysget(U8 *arr) {
    syscall(16, (U32)arr, 0, 0, 0);
}
U0 reboot() {
    syscall(17, 0, 0, 0, 0);
}
U0 poweroff() {
    syscall(18, 0, 0, 0, 0);
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