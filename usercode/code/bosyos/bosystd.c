#include "bosystd.h"
#include <stdint.h>

extern uint32_t _syscall(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e);

__attribute__((cdecl)) uint32_t syscall(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e) { // x86 magic
    volatile uint32_t ret;
    ret = _syscall(a, b, c, d, e);
    return ret;
}
uint32_t strlen(const char *str) {
    uint32_t i = 0;
    while (*str) {
        ++str;
        ++i;
    }
    return i;
}
uint32_t strcmp(const char *a, const char *b) {
    while (*b && (*a == *b)) {++a; ++b;}

    return (uint32_t)(*a-*b);
}
void exit(uint32_t code) {
    syscall(1, code, 0, 0, 0);
}
uint32_t read(uint32_t fd, byte_t *buf, uint32_t count) {
    return syscall(3, fd, (uint32_t)buf, count, 0);
}
uint32_t write(uint32_t fd, byte_t *buf, uint32_t count) {
    return syscall(4, fd, (uint32_t)buf, count, 0);
}
filedesc_t open(const char *filename) {
    return syscall(5, (uint32_t)filename, 0, 0, 0);
}
void close(filedesc_t fd) {
    syscall(4, fd, 0, 0, 0);
}
pid_t execa(void *func) {
    return syscall(11, (uint32_t)func, 0, 0, 0);
}
size_t ioctl(filedesc_t fd, uint32_t req, uint32_t *a1, uint32_t *a2) {
    return syscall(54, fd, req, (uint32_t)a1, (uint32_t)a2);
}
void print(const char *str) {
    write(1, str, strlen(str));
}