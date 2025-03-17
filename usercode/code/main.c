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
                    "help, cat, cls, game\n"
                    "beep\n"
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
                print("// TODO: fix beep");
                // print("\x07");
            }
            else if (!strcmp(buf, "game")) {
                char buf1[53*33+1];
                uint32_t x = 0;
                time_t time_last;
                time_t time_curr;
                time(&time_last);
                for (;;) {
                    do {
                        time(&time_curr);
                    } while (time_curr.real - time_last.real < 100);
            
                    time(&time_last);
                    buf1[0] = 0x80;
                    for (int i = 0; i < sizeof(buf1)-1; ++i) {
                        buf1[i+1] = '.';
                    }
                    buf1[x+10*53+1] = '#';
                    x = (x + 1) % 53;
                    print(buf1);
                }
            }
            else if (!strcmp(buf, "cls")) {
                print("\x80");
            }
            print("$ ");
        }
    }
}