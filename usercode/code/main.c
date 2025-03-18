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

void shell(char *buf);

void shella() {
    shell(c);
    for(;;);
    exit(0);
}

void shell(char *buf) {
    int s = strlen(buf);
    if (buf[s-1] == '\n') { // POSIX compatible
        buf[s-1] = 0;
    }

    if (buf[0] == '&') {
        c = buf + 1;
        execa(shella);
    }
    else if (!strcmp(buf, "help")) {
        print(
            "simple BosyOS shell\n"
            "commands:\n"
            "help, cat, clear, game\n"
            "beep, ls, cls, time\n"
            "ser\n"
        );
    }
    else if (!strcmp(buf, "exit"))
    {
        exit(0);
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
        print("\x07\n");
    }
    else if (!strcmp(buf, "win")) {
        int win = ioctl(3, WIOCREAT, (uint32_t*)50, (uint32_t*)50, (uint32_t*)"Window");
    }
    else if (!strcmp(buf, "game")) {
        char buf[53*33];
        for (int i = 0; i < sizeof(buf); ++i) {
            buf[i] = ' ';
        }
        buf[32*53] = '#';
        char stop = 0;
        while (!stop) {
            for (int i = 0; i < 32; ++i) {
                if (buf[53*(i+1)] == '#') {
                    buf[53*i] = (buf[53*i] != '#') ? '#' : ' ';
                }
            }
            for (int j = 0; j < 33; ++j) {
                for (int i = 52; i > 0; --i) {
                    buf[i+j*53] = buf[i-1+j*53];
                }
            }
            write(1, buf, sizeof(buf));
            
            time_t t1, t2;
            struct time_spec ts;
            clock_gettime(&ts);
            t1 = ts.sec * 1000000000 + ts.nsec;
            t2 = t1;
            while (t2 - t1 < 100000000) {
                clock_gettime(&ts);
                t2 = ts.sec * 1000000000 + ts.nsec;
            }
        }
    }
    else if (!strcmp(buf, "time")) {
        time_t t;
        time(&t);
        printf("Unix time: %d\n", t);
    }
    else if (!strcmp(buf, "ser")) {
        int tty;
        ioctl(1, 92, &tty, 0, 0);
        ioctl(1, 91, 0, 0, 0);
        print("Hello, serial!\n");
        ioctl(1, 91, (uint32_t*)tty, 0, 0);
    }
    else if (!strcmp(buf, "cls") || !strcmp(buf, "clear")) {
        print("\x80");
    }
    else {
        printf("Unk \"%s\"\n", buf);
    }
}

void _main() {
    print("$ ");
    char buf[64];
    for (;;) {
        for (int i = 0; i < 64; ++i) {
            buf[i] = 0;
        }
        if (read(0, buf, 64)) {
            shell(buf);
            print("$ ");
        }
    }
    for(;;);
}

void main() {
    execa(_main);
    ioctl(1, 91, (uint32_t*)2, 0, 0);
    execa(_main);
    ioctl(1, 91, (uint32_t*)3, 0, 0);
    execa(_main);
    ioctl(1, 91, (uint32_t*)4, 0, 0);
    
    _main();
    for(;;);
}