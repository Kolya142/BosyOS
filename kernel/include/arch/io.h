#pragma once
#include <libs/types.h>

#define INT_DEF(fn) __attribute__((naked)) U0 fn()
#define INT_START  asmv("pusha\npushf")
#define INT_RETURN POut(0x20, 0x20);asmv("popf\npopa\niret")

U0 POut(U16 port, U8 data);
U8 PIn(U16 port);