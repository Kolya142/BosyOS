#include <stdint.h>

uint32_t drvcall(uint32_t d1, uint32_t d2, uint32_t id, uint32_t value);
uint32_t syscall(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e);
extern uint32_t _syscall(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e);

float sin(float x);
float cos(float x);
float sqrt(float x);