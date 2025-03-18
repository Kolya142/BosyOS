#include "bosyos/bosystd.h"
#include "bosyos/bsfexe.h"

void lsfn(const char *filename, stat_t *stat) {
    print(filename);
    if (stat->mode & FS_DEV) {
        print(" \t\t| DEV\n");
    }
    else {
        printf(" \t\t| %d bytes\n", stat->size);
    }
}

char *c;

void shell(const char *buf);

void shella() {
    shell(c);
    exit(0);
}

void shell(const char *buf) {
    if (buf[0] == '&') {
        // c = buf + 1;
        // execa(shella);
    }
    else if (!strcmp(buf, "help")) {
        print(
            "simple BosyOS shell\n"
            "commands:\n"
            "help, cat, clear, game\n"
            "beep, ls, cls, time\n"
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
        print("// TODO: add game\n");
    }
    else if (!strcmp(buf, "time")) {
        time_t t;
        time(&t);
        printf("Real time: %d\nUnix time: %d\n", t.real, t.unixt);
    }
    else if (!strcmp(buf, "cls") || !strcmp(buf, "clear")) {
        print("\x80");
    }
    else {
        printf("Unk %s\n", buf);
    }
}

void main() {
    print("\x86\xA6$\x86\xAB ");
    char buf[64];
    for (;;) {
        if (read(0, buf, 64)) {
            shell(buf);
            print("\x86\xA6$\x86\xAB ");
        }
    }
}