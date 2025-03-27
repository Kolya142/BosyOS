#pragma once
#include "bosyos/bosystd.h"

#define MZD_LINK 0
#define MZD_FG 1
#define MZD_BG 2
#define MZD_CLS 3
#define MZD_BTN 4
#define MZD_EXEC 5

typedef struct {
    int type;
    char a1[64];
    char a2[64];
} mzd_object;

typedef struct {
    char name[64];
    char link[64];
    int type;
    int mod;
} mzd_ctx;

int mzd_parse(mzd_ctx *ctx, mzd_object *obj, char c);