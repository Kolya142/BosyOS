#include "drivers.h"
#include <stdint.h>

typedef struct stat_t {
    uint32_t time;
    uint32_t ino;
    uint32_t size;
    uint32_t mode;
} stat_t;

typedef struct time_t {
    uint32_t millis;
    uint32_t unixt;
    uint32_t real;
} time_t;

#define FS_DEV 0x40000000
#define FS_REG 0x20000000
#define FS_UREAD 0x10000000
#define FS_UWRIT 0x08000000
#define FS_UEXEC 0x04000000
#define FS_OREAD 0x02000000
#define FS_OWRIT 0x01000000
#define FS_OEXEC 0x00800000

typedef unsigned char byte_t;
typedef uint32_t filedesc_t;
typedef uint32_t size_t;
typedef uint32_t pid_t;

uint32_t syscall(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f, uint32_t g);

uint32_t strlen(const char *str);
uint32_t strcmp(const char *a, const char *b);
void printf(const char *fmt, ...);

void drivercall(uint32_t d1, uint32_t d2, uint32_t id, uint32_t data);
void exit(uint32_t code);
uint32_t read(uint32_t fd, byte_t *buf, uint32_t count);
uint32_t write(uint32_t fd, byte_t *buf, uint32_t count);
filedesc_t open(const char *filename);
void close(filedesc_t fd);
pid_t execa(void *func);
size_t ioctl(filedesc_t fd, uint32_t req, uint32_t *a1, uint32_t *a2);
void print(const char *str);
void readdir(void(*reader)(const char *filename, stat_t *stat));
void time(time_t *t);