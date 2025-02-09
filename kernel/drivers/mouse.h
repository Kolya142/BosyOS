#include <arch/ports.h>
#include <arch/sleep.h>
#include <drivers/screen.h>
#include <lib/screenlib.h>
#pragma once
extern int MouseX;
extern int MouseY;
extern U8 MouseButtons;
void PS2Write(U16 port, U8 data);
U8 PS2Read(U16 port);
void MouseInit();
void MouseUpd();