#include "bosystd.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>

extern uint32_t _syscall(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f, uint32_t g);

void print_mezc(char c) {
    static bool lbs = false;
    static bool lds = false; // $
    static char   ldv = 0    ;
    static char   lbd = 0    ;
    static char   lbp = 0    ;
    if (lbs) {
        if (c == '\\') 
            print("\\");
        if (c == 'n')
            print("\n");
        if (c == '$') 
            print("$"); 
        lbs = false;
    }
    else if (lds) {
        if (c == '!') {
            ldv = 1;
        }
        else if (c == '*') {
            ldv = 2;
        }
        else if (c == 'x') {
            lbp = 1;
        }
        else if (lbp) {
            lbd <<= 4;
            unsigned char d = 255;
            if (c >= '0' && c <= '9') {
                d = c - '0';
            }
            else if (c >= 'A' && c <= 'F') {
                d = c - 'A' + 10;
            }
            else if (c >= 'a' && c <= 'f') {
                d = c - 'a' + 10;
            }
            lbd |= d;
            lbp ++;
            if (lbp == 3) {
                write(1, (byte_t[]) {0xAC, lbd}, 2);
                lbd = 0;
                lds = false;
                lbp = 0;
            }
        }
        else {
            unsigned char d = 255;
            if (c >= '0' && c <= '9') {
                d = c - '0';
            }
            else if (c >= 'A' && c <= 'F') {
                d = c - 'A' + 10;
            }
            else if (c >= 'a' && c <= 'f') {
                d = c - 'a' + 10;
            }
            if (d - 255) {
                switch (ldv) {
                    case 1: {
                        char buf[2];
                        buf[0] = 0x86;
                        buf[1] = 0x9C + d;
                        write(1, buf, 2);
                    } break;
                    case 2: {
                        char buf[2];
                        buf[0] = 0x87;
                        buf[1] = 0x9C + d;
                        write(1, buf, 2);
                    } break;
                }
            }
            ldv = 0;
            lds = false;
        }
    }
    else if (c & 0x80) {
        char buf[2];
        buf[0] = 0xAC;
        buf[1] = c;
        write(1, buf, 2);
    }
    else if (c == '\\') {lbs = true;}
    else {
        if (c == '$') {
            lds = true;
        } 
        else {
            write(1, &c, 1);
        }
    }
}
void print_mez(char *s) {
    while (*s) {
        print_mezc(*s);
        ++s;
    }
}
void draw_win(int x, int y, int w, int h, const char *title, char *buf) {
    struct time_spec t;
    clock_gettime(&t);

    buf[x + y * 80] = '\x83';
    buf[x + y * 80 + w] = '\x85';

    buf[x + (y + h - 1) * 80] = '\xA5';
    buf[x + (y + h - 1) * 80 + w] = '\x86';
    for (int i = 1; i < w; ++i) {
        buf[x + y * 80 + i] = '\xA4';
        buf[x + (y + h - 1) * 80 + i] = '\xA4';
    }
    for (int i = 1; i < h - 1; ++i) {
        buf[x + (y + i) * 80] = '\xA3';
        buf[x + (y + i) * 80 + w] = '\xA3';
    }
    int s = strlen(title);
    for (int i = 0; i < w-1; ++i) {
        buf[x + 1 + i + y * 80] = title[(t.sec + i) % s];
    }
}

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
void memset(void *ptr, int v, int c) {
    for (;c;--c,++ptr) *((byte_t*)ptr) = v;
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
    // for(;;);
}
uint32_t read(uint32_t fd, byte_t *buf, uint32_t count) {
    return syscall(3, fd, (uint32_t)buf, count, 0, 0, 0);
}
uint32_t write(uint32_t fd, byte_t *buf, uint32_t count) {
    return syscall(4, fd, (uint32_t)buf, count, 0, 0, 0);
}
filedesc_t open(const char *filename) {
    filedesc_t fd = syscall(5, (uint32_t)filename, 0, 0, 0, 0, 0);
    return (fd == 0xFFFFFFFE) ? 0 : fd;
}
void close(filedesc_t fd) {
    syscall(6, fd, 0, 0, 0, 0, 0);
}
pid_t execa(void *func) {
    return syscall(11, (uint32_t)func, 0, 0, 0, 0, 0);
}
void readdir(const char *dir, void(*reader)(const char *filename, stat_t *stat)) {
    syscall(12, (uint32_t)dir, (uint32_t)reader, 0, 0, 0, 0);
}
void time(time_t *t) {
    syscall(13, (uint32_t)t, 0, 0, 0, 0, 0);
}
void lseek(filedesc_t fd, uint32_t off, uint32_t whence) {
    syscall(19, fd, off, whence, 0, 0, 0);
}
int is_bosy() {
    return ioctl(1, 40, 0, 0, 0) == 0xBEEFB0B0;
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