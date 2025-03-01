#include "bosyos/drivers.h"
#include "bosyos/bsfexe.h"
#include "bosystd.h"

uint16_t tones[] = { 79, 78, 76, 76, 78, 57, 62, 66, 62, 59, 62, 66, 62, 57, 62, 66, 69, 62, 79, 59, 78, 62, 76, 66, 76, 62, 57, 78, 62, 66, 62, 74, 59, 62, 76, 66, 69, 62, 57, 62, 66, 62, 59, 62, 66, 69, 62, 76, 59, 64, 78, 67, 79, 64, 59, 64, 76, 67, 73, 64, 57, 74, 64, 67, 64, 76, 57, 64, 69, 67, 69, 64, 57, 78, 62, 66, 62, 59, 62, 66, 62, 57, 62, 66, 62, 79, 59, 78, 62, 76, 66, 76, 62, 78, 57, 62, 66, 62, 59, 62, 66, 62, 57, 62, 66, 69, 62, 79, 59, 78, 62, 76, 66, 76, 62, 57, 62, 78, 66, 74, 62, 59, 62, 76, 66, 69, 62, 57, 62, 66, 62, 59, 62, 66, 62, 76, 59, 64, 78, 67, 79, 64, 59, 64, 76, 67, 73, 64, 57, 64, 74, 67, 76, 64, 57, 69, 64, 74, 67, 76, 64, 77, 58, 76, 62, 74, 65, 72, 70, 69, 70, 72, 53, 60, 77, 65, 60, 76, 55, 74, 62, 74, 65, 72, 62, 74, 57, 72, 60, 72, 65, 60, 72, 53, 60, 69, 65, 70, 60, 72, 53, 60, 77, 65, 60, 79, 55, 77, 62, 76, 65, 74, 62, 74, 57, 76, 60, 77, 65, 60, 77, 57, 60, 79, 65, 81, 60, 82, 58, 82, 65, 81, 69, 65, 79, 58, 65, 77, 69, 79, 65, 81, 57, 81, 60, 79, 65, 60, 77, 57, 60, 74, 65, 72, 60, 74, 57, 77, 60, 77, 65, 76, 60, 57, 76, 60, 78, 64, 78, 66, 50, 57, 78, 74, 50, 57, 71, 47, 54, 74, 78, 74, 47, 54, 69 };
uint16_t durations[] = { 1010, 249, 246, 246, 249, 0, 264, 248, 246, 249, 249, 246, 246, 249, 264, 248, 246, 0, 249, 0, 249, 0, 246, 0, 246, 0, 249, 264, 0, 248, 246, 249, 0, 249, 246, 0, 246, 0, 249, 264, 248, 246, 249, 249, 246, 246, 0, 249, 0, 264, 248, 0, 246, 0, 249, 249, 246, 0, 246, 0, 249, 264, 0, 248, 246, 249, 0, 249, 246, 0, 246, 0, 249, 264, 0, 248, 246, 249, 249, 246, 246, 249, 264, 248, 246, 249, 0, 249, 0, 246, 0, 246, 0, 249, 0, 264, 248, 246, 249, 249, 246, 246, 249, 264, 248, 246, 0, 249, 0, 249, 0, 246, 0, 246, 0, 249, 264, 248, 0, 246, 0, 249, 249, 246, 0, 246, 0, 249, 264, 248, 246, 249, 249, 246, 246, 249, 0, 264, 248, 0, 246, 0, 249, 249, 246, 0, 246, 0, 249, 264, 248, 0, 246, 0, 249, 249, 0, 246, 0, 246, 0, 249, 0, 264, 0, 248, 0, 246, 0, 744, 246, 249, 0, 264, 248, 0, 246, 249, 0, 249, 0, 246, 0, 246, 0, 249, 0, 264, 0, 248, 0, 246, 249, 0, 249, 246, 0, 246, 0, 249, 0, 264, 248, 0, 246, 249, 0, 249, 0, 246, 0, 246, 0, 249, 0, 264, 0, 248, 0, 246, 249, 0, 249, 246, 0, 246, 0, 249, 0, 264, 0, 248, 0, 246, 249, 0, 249, 246, 0, 246, 0, 249, 0, 264, 0, 248, 0, 246, 249, 0, 249, 246, 0, 246, 0, 249, 0, 264, 0, 248, 0, 246, 0, 249, 249, 0, 246, 0, 246, 0, 249, 0, 512, 246, 0, 0, 249, 0, 0, 249, 246, 246, 0, 0, 249 };

void mainer() {

    syscall(1, (uint32_t)"Hello, world!\n", 0, 0, 0);
    // for (uint32_t i = 0; i < sizeof(tones) / 2; ++i) {
    //     drvcall(DRVBEEP, 2, (tones[i] << 16) | durations[i]);
    //     syscall(4, 1000/120, 0, 0, 0);
    // }
    exit(0);
}

void mainer1() {
    for (uint32_t i = 0; i < sizeof(tones) / 2; ++i) {
        // syscall(8, tones[i], durations[i], 0, 0);
        drvcall(DBEEP, 2, ((tones[i]) << 16) | durations[i]);
        syscall(4, 1000/120, 0, 0, 0);
    }
    exit(0);
}

void mainer2() {
    String str = "\nSERIAL2VGA START: ";
    for (U32 i = 0; str[i]; ++i)
        drvcall(DSERIAL, 0, str[i]);
    for (;;) {
        U8 c = drvcall(DSERIAL, 1, 0);
        syscall(6, c, 0, 0, 0);
    }
    exit(0);
}

void game() {
    String hello = "Hello from my game\n";
    syscall(1, (U32)hello, 0, 0, 0);
    U32 x = 40;
    U32 y = 12;
    String map = 
        "                                                                                "
        "                                                                                "
        "                                                                                "
        "                                                                                "
        "                                                                                "
        "                                                                                "
        "                                                   ####################         "
        "                  ##       ##                       #                #          "
        "                                                     #              #           "
        "                                                      #            #            "
        "                               #              #        #          #             "
        "                                                        #        #              "
        "                                   #   #                ##########              "
        "                                                               #                "
        "                                           #                  #                 "
        "                                                             #                  "
        "                                                   #        #                   "
        "                                                  #        #                    "
        "                                                 #    #####                     "
        "                                                #                               "
        "                                               #                                "
        "            #  #   ###                        #                                 "
        "             #  #                            #                                  "
        "            #  #   ###                      #                                   "
        "################################################################################";
    for (;;) {
        Char c = drvcall(DKEYBOARD, 0, 0);
        if (!(drvcall(DKEYBOARD, 1, 0) & 0x80)) { // Check key press
            switch (c) {
                case 'a': --x; break;
                case 'd': ++x; break;
                case '\x1b': exit(0);
            }
        }
        if (map[x + y * 80] == '#') {
            --y;
        }
        else if (map[x + y * 80 + 80] != '#') {
            ++y;
        }
        for (U32 i = 0; i < 2000; ++i) {
            Char ch;
            if (i == x + y * 80) {
                ch = 'p';
            }
            else if (map[i] == '#') {
                ch = '#';
            }
            else {
                ch = ' ';
            }
            ((U16*)0xB8000)[i] = 0x0F00 | ch;
        }
        syscall(4, 100, 0, 0, 0);
    }
    exit(0);
}


FUNCTION(mainer, 0, 1)
FUNCTION(mainer1, 0, 2)
FUNCTION(mainer2, 0, 3)
FUNCTION(game, 0, 4)