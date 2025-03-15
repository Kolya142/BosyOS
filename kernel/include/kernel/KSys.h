#pragma once
#include <lib/Types.h>

typedef enum KUserType {
    KUserReg,
    KUserDev,
} KUserType;

typedef struct KUser {
    KUserType type;
    String username
} KUser;

extern KUser UserCurr;