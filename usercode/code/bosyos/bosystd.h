#include "drivers.h"
#include <stdint.h>

typedef unsigned char byte_t;
typedef uint32_t filedesc_t;
typedef uint32_t size_t;
typedef uint32_t time_t;
typedef uint32_t pid_t;

uint32_t syscall(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e);
uint32_t strlen(const char *str);
uint32_t strcmp(const char *a, const char *b);
void exit(uint32_t code);
uint32_t read(uint32_t fd, byte_t *buf, uint32_t count);
uint32_t write(uint32_t fd, byte_t *buf, uint32_t count);
filedesc_t open(const char *filename);
void close(filedesc_t fd);
pid_t execa(void *func);
size_t ioctl(filedesc_t fd, uint32_t req, uint32_t *a1, uint32_t *a2);
void print(const char *str);