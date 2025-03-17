#include "bosyos/bosystd.h"
#include "bosyos/bsfexe.h"

void lsfn(const char *filename, stat_t *stat) {
    print(filename);
    print("\n");
}

void main() {
    print("$ ");
    char buf[64];
    for (;;) {
        if (read(0, buf, 64)) {
            if (!strcmp(buf, "help")) {
                print(
                    "simple BosyOS shell\n"
                    "commands:\n"
                    "help, cat, clear, game\n"
                    "beep, ls\n"
                );
            }
            else if (buf[0] == 'c' && buf[1] == 'a' && buf[2] == 't') {
                filedesc_t fd = open(buf + 4);
                if (fd) {
                    uint32_t count = read(fd, buf, 64);
                    print("file content:\n");
                    write(1, buf, count);
                    print("\n");
                    close(fd);
                }
                else {
                    print("file not fount\n");
                }
            }
            else if (!strcmp(buf, "ls")) {
                readdir(lsfn);
            }
            else if (!strcmp(buf, "beep"))
            {
                print("// TODO: fix beep\n");
                // print("\x07");
            }
            else if (!strcmp(buf, "game")) {
                print("// TODO: fix game\n");
                // char buf1[53*33+1];
                // int x = 0;
                // int y = 0;
                // time_t time_last;
                // time_t time_curr;
                // time(&time_last);
                // uint8_t keys[256];
                // for (;;) {
                //     do {
                //         time(&time_curr);
                //     } while (time_curr.real - time_last.real < 100);
            
                //     time(&time_last);
                //     buf1[0] = 0x80;
                //     for (int i = 0; i < sizeof(buf1)-1; ++i) {
                //         buf1[i+1] = '.';
                //     }
                //     buf1[x+y*53+1] = '@';

                //     drivercall(DKEYBOARD, 3, (uint32_t)keys);

                //     if (keys['w']) {
                //         --y;
                //     }
                //     if (keys['a']) {
                //         --x;
                //     }
                //     if (keys['s']) {
                //         ++y;
                //     }
                //     if (keys['d']) {
                //         ++x;
                //     }

                //     print(buf1);
                // }
            }            
            else if (!strcmp(buf, "clear")) {
                print("\x80");
            }
            print("$ ");
        }
    }
}