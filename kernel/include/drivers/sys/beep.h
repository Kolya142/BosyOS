#pragma once
#include <arch/x86/cpu/io.h>
#include <lib/time/Time.h>
U0 Beep(U16 dur);
U0 BeepHz(U16 freq, U16 dur);
U0 BeepSPC(U8 tone, U16 dur);
U0 BeepInit();