#pragma once
#include <stdint.h>
struct bosy_metadata {
    uint32_t meta1;
    uint32_t meta2;
    void *func;
} __attribute__((packed));
#define FUNCTION(func, meta1, meta2) __attribute__((section(".bossec"), used)) const static struct bosy_metadata bosy_entry_##func = {meta1, meta2, func};