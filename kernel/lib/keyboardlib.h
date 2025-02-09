#include <drivers/screen.h>
#include <drivers/kerboard.h>
#include <lib/asciip.h>
#include <lib/string.h>
#include <arch/halt.h>

extern char KeyboardLibLastKeyState;
extern int KeyboardLibKeyTimer;
extern U8 KeyboardLibLastKey;
int KeyboardRead(char *buf, int count);
