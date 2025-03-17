#include "bosyos/bosystd.h"
#include "bosyos/bsfexe.h"


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
            else if (!strcmp(buf, "game")) {
                print("// TODO: add game\n");
            }
            else if (!strcmp(buf, "cls")) {
                print("\x80");
            }
            print("$ ");
        }
    }
}