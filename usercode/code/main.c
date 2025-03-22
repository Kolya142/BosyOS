#include "bosyos/bosystd.h"

void lsfn(const char *filename, stat_t *stat) {
    print(filename);
    if (stat->mode & FS_DEV) {
        print(" \t\t| DEV\n");
    }
    else {
        printf(" \t\t| %d bytes\n", stat->size);
    }
}

char *ccc;

void shell(char *buf);

void shella() {
    shell(ccc);
    exit(0);
    for(;;);
}

struct utsname {
    char sysname[65];
    char nodename[65];
    char release[65];
    char version[65];
    char machine[65];
};
int screen = 0;
void put_pixel(int x, int y, char c) {
    lseek(screen, x+y*320, 0);
    write(screen, (byte_t[]) {c}, 1);
}

void shell(char *buf) {
    int s = strlen(buf);
    if (buf[s-1] == '\n') { // POSIX compatible
        buf[s-1] = 0;
    }
    
    if (!strcmp(buf, "help")) {
        print(
            "BosyOS shell\n"
            "help  - show this message\n"
            "fetch - show simple system information\n"
            "paint - just paint\n"
            "anim  - show a simple animation\n"
            "cls   - clear screen\n"
            "clear - clear screen\n"
            "tut   - open a simple tutorial\n"
        );
    }
    else if (!strcmp(buf, "tut")) {
        print(
            "Steps:\n"
            "1. releases\n"
            "\n"
            "write something and press enter to start\n"
        );
        
        while (!read(0, (byte_t[]) {0}, 1));
        {
            print(
                "BosyOS releases has 4 types:\n"
                "night         - main & unstable branch\n"
                "stable        - stable branch\n"
                "test          - tests branch\n"
                "user-friendly - for users branch\n"
                "\n"
                "BosyOS release name:\n"
                "BRANCH       KERNEL HASH\n"
                "  |               |     \n"
                "  V               V     \n"
                "night  -  e36613e617249726"
                "\n"
                "write something and press enter to end\n"
            );
        }
        
        while (!read(0, (byte_t[]) {0}, 1));
    }
    else if (!strcmp(buf, "cls") || !strcmp(buf, "clear")) {
        struct utsname name;
        syscall(122, (uint32_t)&name, 0, 0, 0, 0, 0);

        if (!strcmp(name.sysname, "BosyOS")) {
            print("\x80");
        }
        else {
            print("\x1b[2J\x1b[H");
        }
    }
    else if (!strcmp(buf, "fetch")) {
        struct utsname name;
        syscall(122, (uint32_t)&name, 0, 0, 0, 0, 0);

        if (is_bosy()) {
            print("\x80");
        }
        else {
            print("\x1b[2J\x1b[H");
        }
        printf(
            "%s V%s\n"
            "Builded at %s\n"
            ,
            name.sysname,
            name.release,
            name.version
        );
    }
    else if (!strcmp(buf, "paint")) {
        if (screen) {
            while (!read(0, (byte_t[]) {0}, 1)) {
                int mouse[3] = {0};
                ioctl(3, 50, mouse, 0, 0);
                if (mouse[2]) {
                    lseek(screen, mouse[0]-1+mouse[1]*320, 0);
                    write(screen, (byte_t[]) {0x0F}, 1);
                }
            }
        }
    }
    else if (!strcmp(buf, "anim")) {
        print("write something and press enter to exit\nwrite something and press enter to start");
        while (!read(0, (byte_t[]) {0}, 1));
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
            if (read(0, (byte_t[]) {0}, 1)) {
                stop = 1;
            }
        }
    }
    else if (!strcmp(buf, "graph")) {
        for (int x = 0; x < 3200; ++x) {
            int i = (x / 10) - 320/2;
            put_pixel(x / 10, 200 - (i*i) / 10, 0x0F);
        }
    }
}

void _start() {
    if (is_bosy()) {
        screen = open("/dev/screen");
    }
    print("Welcome to BosyOS shell\n");
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