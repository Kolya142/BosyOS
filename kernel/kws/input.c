#include <drivers/input/keyboard.h>
#include <drivers/input/mouse.h>
#include <kws/input.h>

U0 KWSUpdate(KWSInputData *inp) {
    inp->mousex = MouseX;
    inp->mousey = MouseY;
    inp->mouse_left = inp->mouse_down[0] ? (False) : ((MouseBtn & 1) ? True : False);
    inp->mouse_right = inp->mouse_down[1] ? (False) : ((MouseBtn & 2) ? True : False);
    inp->mouse_middle = inp->mouse_down[2] ? (False) : ((MouseBtn & 4) ? True : False);

    inp->mouse_down[0] = (MouseBtn & 1) ? True : False;
    inp->mouse_down[1] = (MouseBtn & 2) ? True : False;
    inp->mouse_down[2] = (MouseBtn & 4) ? True : False;

    for (U32 i = 0; i < 256; ++i) {
        if (KBState.keys[i] && !inp->key_down[i]) {
            inp->key_pressed[i] = True;
        }
        else {
            inp->key_pressed[i] = False;
        }
        inp->key_down[i] = KBState.keys[i];
    }
}