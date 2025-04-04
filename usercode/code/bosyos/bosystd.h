#pragma once
#include "drivers.h"
#include <stdint.h>

typedef struct stat_t {
    uint32_t time;
    uint32_t ino;
    uint32_t size;
    uint32_t mode;
} stat_t;
struct time_spec {
    uint32_t sec;
    uint32_t nsec;
};
// typedef struct piel_canvas {
//     uint8_t *arr;
//     uint32_t rwidth;
//     uint32_t width;
//     uint32_t height;
// } piel_canvas;

// typedef struct kws_input {
//     int mousex;
//     int mousey;
//     char mouse_left;
//     char mouse_right;
//     char mouse_middle;
//     char mouse_down[3];
//     char key_down[256];
//     char key_pressed[256];
// } kws_input;

// typedef struct win
// {
//     char title[16];
//     uint32_t x;
//     uint32_t y;
//     uint32_t w;
//     uint32_t h;
//     uint16_t flags;
//     piel_canvas canvas;
//     void(*update)(void *this);
//     kws_input inp;
// } win;

#define FS_DEV 0x40000000
#define FS_REG 0x20000000
#define FS_UREAD 0x10000000
#define FS_UWRIT 0x08000000
#define FS_UEXEC 0x04000000
#define FS_OREAD 0x02000000
#define FS_OWRIT 0x01000000
#define FS_OEXEC 0x00800000

#define TIOWINSZ 54
#define TIOGTCUR 55
#define TIOSTTTY 91
#define TIOGTTTY 92
#define TIONWTTY 63
#define TIONWPTY 62

#define WIOCREAT 10
#define WIOKILL  11
#define WIOUPDFN 20

typedef unsigned char byte_t;
typedef uint32_t filedesc_t;
typedef uint32_t time_t;
typedef uint32_t size_t;
typedef uint32_t pid_t;
void print_mezc(char c);
void print_mez(char *s);
void draw_win(int x, int y, int w, int h, const char *title, char *buf);

uint32_t syscall(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f, uint32_t g);

uint32_t strlen(const char *str);
void memset(void *ptr, int v, int c);
uint32_t strcmp(const char *a, const char *b);
void printf(const char *fmt, ...);

void drivercall(uint32_t d1, uint32_t d2, uint32_t id, uint32_t data);
void exit(uint32_t code);
uint32_t read(uint32_t fd, byte_t *buf, uint32_t count);
uint32_t write(uint32_t fd, byte_t *buf, uint32_t count);
filedesc_t open(const char *filename);
void close(filedesc_t fd);
pid_t execa(void *func);
int is_bosy();
size_t ioctl(filedesc_t fd, uint32_t req, uint32_t *a1, uint32_t *a2, uint32_t *a3);
void print(const char *str);
void readdir(const char *dir, void(*reader)(const char *filename, stat_t *stat));
void time(time_t *t);
void lseek(filedesc_t fd, uint32_t off, uint32_t whence);
void clock_gettime(struct time_spec *ts);