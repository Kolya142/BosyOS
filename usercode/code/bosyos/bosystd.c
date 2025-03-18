#include "bosystd.h"
#include <stdint.h>
#include <stdarg.h>

extern uint32_t _syscall(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f, uint32_t g);

__attribute__((cdecl)) uint32_t syscall(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f, uint32_t g) { // x86 magic
    volatile uint32_t ret;
    ret = _syscall(a, b, c, d, e, f, g);
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
void drivercall(uint32_t d1, uint32_t d2, uint32_t id, uint32_t data) {
    syscall(0, d1, d2, id, data, 0, 0);
}
void exit(uint32_t code) {
    syscall(1, code, 0, 0, 0, 0, 0);
}
uint32_t read(uint32_t fd, byte_t *buf, uint32_t count) {
    return syscall(3, fd, (uint32_t)buf, count, 0, 0, 0);
}
uint32_t write(uint32_t fd, byte_t *buf, uint32_t count) {
    return syscall(4, fd, (uint32_t)buf, count, 0, 0, 0);
}
filedesc_t open(const char *filename) {
    return syscall(5, (uint32_t)filename, 0, 0, 0, 0, 0);
}
void close(filedesc_t fd) {
    syscall(4, fd, 0, 0, 0, 0, 0);
}
pid_t execa(void *func) {
    return syscall(11, (uint32_t)func, 0, 0, 0, 0, 0);
}
void readdir(void(*reader)(const char *filename, stat_t *stat)) {
    syscall(12, (uint32_t)reader, 0, 0, 0, 0, 0);
}
void time(time_t *t) {
    syscall(13, (uint32_t)t, 0, 0, 0, 0, 0);
}
size_t ioctl(filedesc_t fd, uint32_t req, uint32_t *a1, uint32_t *a2, uint32_t *a3) {
    return syscall(54, fd, req, (uint32_t)a1, (uint32_t)a2, (uint32_t)a3, 0);
}
void clock_gettime(struct time_spec *ts) {
    syscall(265, 0, (uint32_t)ts, 0, 0, 0, 0);
}


void print(const char *str) {
    write(1, str, strlen(str));
}

void print_dec(uint32_t a) {
    if (!a) {
        print("0");
        return;
    }
    char buf[11] = {0};
    uint32_t p = 10;
    buf[p] = 0;

    while (a) {
        buf[--p] = '0' + (a % 10);
        a /= 10;
    }

    for (uint32_t i = 0; i < 11; ++i) {
        if (buf[i]) {
            syscall(4, 1, (uint32_t)&buf[i], 1, 0, 0, 0);
        }
    }
}
void print_deci(int32_t a) {
    if (a < 0) {
        print("-");
        a = -a;
    }
    print_dec(a);
}
void printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    uint32_t n = 4;
    while (*fmt) {
        if (*fmt == '%' && *(fmt + 1)) {
            ++fmt;
            if (*fmt >= '0' && *fmt <= '4') {
                n = *fmt - '0';
                ++fmt;
            }
            switch (*fmt) {
                case 'c': {
                    char c = va_arg(args, uint32_t);
                    syscall(4, 1, (uint32_t)&c, 1, 0, 0, 0);
                } break;
                case 'C': {
                    char c = va_arg(args, uint32_t);
                    syscall(4, 1, (uint32_t)&c, 1, 0, 0, 0);
                } break;
                case 's': {
                    const char * s = va_arg(args, const char *);
                    if (!s) s = "(null)";
                    print(s);
                } break;
                case 'B': {
                    char s = va_arg(args, uint32_t);
                    if (s) {
                        print("True");
                    }
                    else {
                        print("False");
                    }
                } break;
                case 'b': {
                    uint16_t s = va_arg(args, uint32_t);
                    for (int8_t i = n * sizeof(uint8_t) - 1; i >= 0; --i) {
                        char c = (s & (1 << i)) ? '1' : '0';
                        syscall(4, 1, (uint32_t)c, 1, 0, 0, 0);
                    }
                } break;                
                case 'd': {
                    uint32_t s = va_arg(args, uint32_t);
                    print_dec(s);
                } break;
                case 'i': {
                    int32_t s = va_arg(args, int32_t);
                    print_deci(s);
                } break;
                case 'p': {
                    uint32_t s = va_arg(args, uint32_t);
                    print("0x");
                } break;
                case 'z': {
                    uint32_t s = va_arg(args, uint32_t);
                } break;
                case 'x': {
                    uint32_t s = va_arg(args, uint32_t);
                    for (uint8_t i = n * 2; i > 0; --i) {
                        char c = "0123456789abcdef"[(s >> ((i - 1) * 4)) & 0xF];
                        syscall(4, 1, (uint32_t)&c, 1, 0, 0, 0);
                    }
                } break;
                case 'X': {
                    uint32_t s = va_arg(args, uint32_t);
                    for (uint8_t i = n * 2; i > 0; --i) {
                        char c = "0123456789ABCDEF"[(s >> ((i - 1) * 4)) & 0xF];
                        syscall(4, 1, (uint32_t)&c, 1, 0, 0, 0);
                    }
                } break;
                case '%': {
                    print("%");
                } break;
                default:
                    print("%");
                    syscall(4, 1, (uint32_t)fmt, 1, 0, 0, 0);
            }
            n = 4;
            ++fmt;
            continue;
        }
        syscall(4, 1, (uint32_t)fmt, 1, 0, 0, 0);
        ++fmt;
    }
}