#pragma once
#include <lib/Types.h>
#include <lib/time/Time.h>
#include <lib/IO/TTY.h>

typedef struct KWSInputData {
    I32 mousex;
    I32 mousey;
    Bool mouse_left;
    Bool mouse_right;
    Bool mouse_middle;
    Bool mouse_down[3];
    Bool key_down[256];
    Bool key_pressed[256];
} KWSInputData;

U0 KWSUpdate(KWSInputData *inp);