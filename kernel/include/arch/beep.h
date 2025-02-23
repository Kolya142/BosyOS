#pragma once
#include <arch/io.h>
#include <lib/Time.h>
U0 Beep(U16 dur);
U0 BeepHz(U8 freq, U16 dur);
U0 BeepSPC(U8 tone, U16 dur);