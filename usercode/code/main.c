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

char *ccc;

void shell(char *buf);

void shella() {
    shell(ccc);
    exit(0);
    for(;;);
}

void winupd(win *this) {
    if (this->inp.mouse_left) {
        printf("mouse click\n");
    }
}
struct utsname {
    char sysname[65];
    char nodename[65];
    char release[65];
    char version[65];
    char machine[65];
};

void shell(char *buf) {
    int s = strlen(buf);
    if (buf[s-1] == '\n') { // POSIX compatible
        buf[s-1] = 0;
    }

    if (buf[0] == '&') {
        ccc = &buf[1];
        execa(shella);
    }
    else if (!strcmp(buf, "help")) {
        print(
            "simple BosyOS shell\n"
            "commands:\n"
            "help, cat, clear, game\n"
            "beep, ls, cls, time\n"
            "ser, screen, uname, fetch\n"
        );
    }
    else if (!strcmp(buf, "exit"))
    {
        exit(0);
        for(;;);
    }
    else if (!strcmp(buf, "uname"))
    {
        struct utsname name;
        syscall(122, (uint32_t)&name, 0, 0, 0, 0, 0);
        printf (
            "OS: %s\n"
            "Node: %s\n"
            "Release: %s\n"
            "Version: %s\n"
            "Arch: %s\n"
            ,
            name.sysname,
            name.nodename,
            name.release,
            name.version,
            name.machine
        );
    }
    else if (!strcmp(buf, "fetch")) {
        struct utsname name;
        syscall(122, (uint32_t)&name, 0, 0, 0, 0, 0);

        if (!strcmp(name.sysname, "BosyOS")) {
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
    else if (buf[0] == 'c' && buf[1] == 'a' && buf[2] == 't') {
        filedesc_t fd = (strlen(buf) == 3) ? 0 : open(buf + 4);
        if (fd) {
            uint32_t count;
            print("file content:\n");
            while (count = read(fd, buf, 64)) {
                write(1, buf, count);
                print("\n");
                if (read(0, (byte_t[]) {0}, 1)) {
                    break;
                }
            }
            close(fd);
        }
        else {
            print("file not found\n");
        }
    }
    else if (buf[0] == 'l' && buf[1] == 's') {
        readdir(buf + 3, lsfn);
    }
    else if (!strcmp(buf, "beep"))
    {
        print("\x07\n");
    }
    else if (!strcmp(buf, "win")) {
        int win = ioctl(3, WIOCREAT, (uint32_t*)50, (uint32_t*)50, (uint32_t*)"Window");
        ioctl(3, WIOUPDFN, (uint32_t*)win, (uint32_t*)winupd, 0);
    }
    else if (!strcmp(buf, "screen")) {
        filedesc_t fd = open("/dev/screen");
        if (fd) {
            uint8_t patch[] = "\x05\x00\x05\x00\x05\x00" "\x0A\x0B\x0C\x0D\x0E" "\x0B\x0C\x0D\x0E\x0A" "\x0C\x0D\x0E\x0A\x0B" "\x0D\x0E\x0A\x0B\x0C" "\x0E\x0A\x0B\x0C\x0D";
            write(fd, patch, 31);
            close(fd);
        }
        else {
            print("screen not found\n");
        }
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

void main() {
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
void _main() {
    ioctl(1, 91, (uint32_t*)1, 0, 0);
    execa(main);
    ioctl(1, 91, (uint32_t*)2, 0, 0);
    execa(main);
    ioctl(1, 91, (uint32_t*)3, 0, 0);
    execa(main);
    for(;;);
}