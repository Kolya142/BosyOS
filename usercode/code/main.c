#include "bosyos/bosystd.h"

void lsfn(const char *filename, stat_t *stat) {
    print(filename);
    if (stat->mode & FS_DEV) {
        print("   | DEV");
    }
    else {
        printf("   | %d bytes", stat->size);
    }
    printf(",  mode %d\n", stat->mode);
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
    lseek(screen, x+y*640, 0);
    write(screen, (byte_t[]) {c}, 1);
}

int atoi(const char *str) {
    int n = 0;
    while (*str) {
        if (*str >= '0' && *str <= '9') {
            n *= 10;
            n += *str - '0';
        }
        ++str;
    }
    return n;
}

void shell(char *buf) {
    int s = strlen(buf);
    if (buf[s-1] == '\n') {
        buf[s-1] = 0;
    }
    
    if (buf[0] == '!') {
        execa(buf+1);
    }
    else if (!strcmp(buf, "help")) {
        print(
            "BosyOS shell\n"
            "help  - show this message\n"
            "fetch - show system information\n"
            "paint - just paint\n"
            "put   - print text without \\n\n"
            "uexec - read and exec user's command\n"
            "anim  - show a simple animation\n"
            "cls   - clear screen\n"
            "clear - clear screen\n"
            "tut   - open a simple tutorial\n"
            "waitr - wait when user pressed enter\n"
            "ls    - list files\n"
            "cat   - read file\n"
            "calc  - simple calculator\n"
            "wait  - wait for X millis\n"
            "sleep - enter sleep mode\n"
            "!FILE - run FILE as elf executable\n"
            "bsh   - load bsh script\n"
            "echo  - print text\n"
            "echm  - print mezex without \\n\n"
            "colX  - set X as background color\n"
            "win   - show simple window system\n"
        );
    }
    else if (!strcmp(buf, "waitr")) {
        char c;
        while (!read(0, &c, 1));
    }
    else if (!strcmp(buf, "win")) {
        char buf[80*60] = {0};
        char running = 1;
        int t = 0;
        for (;running;) {
            for (int i = 0; i < 80*60; ++i) {
                buf[i] = 0;
            }
            draw_win(10, 10, 20, 10, "Window", buf);
            int mouse[3] = {0};
            ioctl(3, 50, mouse, 0, 0);
            buf[mouse[0]/8+(mouse[1]/8)*80] = 'M';
            char buf1[80*60*2];
            for (int i = 0; i < 80*60; ++i) {
                buf1[i*2] = 0xAC;
                buf1[i*2+1] = buf[i];
            }
            lseek(1, 0, 0);
            write(1, buf1, sizeof(buf1));
            char c;
            read(0, &c, 1);
            switch (c) {
                case '\n': {
                    running = 0;
                } break;
            }
            ++t;
            time_t t1, t2;
            struct time_spec ts;
            clock_gettime(&ts);
            t1 = ts.sec * 1000000000 + ts.nsec;
            t2 = t1;
            while (t2 - t1 < 10000000) {
                clock_gettime(&ts);
                t2 = ts.sec * 1000000000 + ts.nsec;
            }
        }
    }
    else if (buf[0] == 'c' && buf[1] == 'a' && buf[2] == 'l' && buf[3] == 'c') {
        char *ibuf = buf + 5;
        char tok[13] = {0};
        int v1, v2;
        char expr;

        int i = 0;
        int j = 0;
        int k = 0;
        for (;;) {
            if ((ibuf[k] == ' ') || !ibuf[k]) {
                switch (i) {
                    case 0: {
                        v1 = atoi(tok);
                    } break;
                    case 1: {
                        expr = tok[0];
                    } break;
                    case 2: {
                        v2 = atoi(tok);
                        printf("(%d %c %d)\n", v1, expr, v2);
    
                        switch (expr) {
                            case '-': {
                                v1 = (v1 - v2);
                            } break;
                            case '+': {
                                v1 = (v1 + v2);
                            } break;
                            case '*': {
                                v1 = (v1 * v2);
                            } break;
                            case '/': {
                                v1 = (v1 / v2);
                            } break;
                            case '^': {
                                v1 = (v1 ^ v2);
                            } break;
                            case '&': {
                                v1 = (v1 & v2);
                            } break;
                            case '|': {
                                v1 = (v1 | v2);
                            } break;
                        }
                        i = 0;
                    } break;
                }
                for (int w = 0; w < sizeof(tok); ++w) {tok[w] = 0;}
                ++i;
                j = 0;
            }
            else {
                tok[j] = ibuf[k];
                ++j;
            }
            if (!ibuf[k]) {
                break;
            }
            ++k;
        }
        printf("result: %d\n", v1);
    }
    else if (!strcmp(buf, "tut")) {
        shell("bsh tut.bsh");
    }
    else if (buf[0] == 'e' && buf[1] == 'd') {
        char *fn = buf + 3;
        char buf1[32];
        char tbuf[512];
        int pos = 0;
        for (int i = 0; i < sizeof(tbuf); ++i) {
            tbuf[i] = 0;
        }
        char running = 1;
        int file = open(fn);
        if (!file) {
            printf("Can't open file!\n");
            return;
        }
        read(file, tbuf, 512);
        lseek(file, 0, 0);
        while (running) {
            for (int i = 0; i < sizeof(buf1); ++i) {
                buf1[i] = 0;
            }
            if (read(0, buf1, sizeof(buf1))) {
                switch (buf1[0]) {
                    case 'h': {
                        print (
                            "h - show this text\n"
                            "q - quit\n"
                            "w - write\n"
                            "a - append(rewrite)\n"
                            "s - show from cursor\n"
                            "p - print full text\n"
                            "r - remove line\n"
                            "g - set cursor\n"
                            "G - print cursor\n"
                        );
                    } break;
                    case 'q': {
                        running = 0;
                    } break;
                    case 'w': {
                        write(file, tbuf, strlen(tbuf));
                        close(file);
                        running = 0;
                    } break;
                    case 'a': {
                        int s = strlen(buf1 + 1);
                        for (int i = 0; i < s; ++i) {
                            tbuf[pos] = buf1[i + 1];
                            ++pos;
                        }
                        tbuf[pos] = '\n';
                        ++pos;
                    } break;
                    case 's': {
                        printf("Text from cursor:\n%s\n", tbuf + pos);
                    } break;
                    case 'p': {
                        printf("Full Text:\n%s\n", tbuf);
                    } break;
                    case 'r': {
                        if (pos) {
                            int p1 = pos;
                            tbuf[pos] = 0;
                            --pos;
                            for (;tbuf[pos] != '\n' && pos > 0;--pos) tbuf[pos] = 0;
                            printf("-%d\n", pos - p1);
                        }
                    } break;
                    case 'g': {
                        pos = atoi(buf1 + 1);
                    } break;
                    case 'G': {
                        printf("%d\n", pos);
                    } break;
                }
            }
        }
    }
    else if (buf[0] == 'e' && buf[1] == 'c' && buf[2] == 'h' && buf[3] == 'o') {
        printf("%s\n", buf + 5);
    }
    else if (buf[0] == 'e' && buf[1] == 'c' && buf[2] == 'h' && buf[3] == 'm') {
        print_mez(buf + 5);
    }
    else if (buf[0] == 'b' && buf[1] == 's' && buf[2] == 'h') {
        int file = open(buf + 4);
        // printf("bsh: %d\n", file);
        if (file) {
            int labels[16] = {0};
            char fbuf[2048] = {0};
            read(file, fbuf, 2048);
            // printf("script: \n%s\n", fbuf);
            int i = 0;
            int j = 0;
            char line[64] = {0};
            for (;fbuf[i];++i) {
                if (fbuf[i] == '\n') {
                    line[j] = 0;
                    j = 0;
                    // printf("line: %s\n", line);
                    if (line[0] == ':') {
                        labels[atoi(line + 1)] = i;
                    }
                    else if (line[0] == '#');
                    else if (line[0] == 'g' && line[1] == 't') {
                        i = labels[atoi(line + 3)];
                        while (fbuf[i] != '\n') ++i;
                    }
                    else {
                        shell(line);
                    }
                }
                else if (j < sizeof(line) - 1) {
                    line[j++] = fbuf[i];
                }
            }
            if (j > 0) {
                line[j] = 0;
                shell(line);
            }
            close(file);
        }
        else {
            printf("Could not open file %s\n", buf + 4);
        }
    }
    else if (buf[0] == 't' && buf[1] == 'h') {
        syscall(8, (uint32_t)(buf + 3), 0, 0, 0, 0, 0);
    }
    else if (buf[0] == 'c' && buf[1] == 'a' && buf[2] == 't') {
        filedesc_t fd = open(buf + 4);
        if (fd) {
            print("file content:\n");
            uint32_t count;
            for(;;) {
                count = read(fd, buf, 64);
                if (!count) {
                    break;
                }
                write(1, buf, count);
            }
            print("\n");
            close(fd);
        }
        else {
            print("file not found\n");
        }
    }
    else if (buf[0] == 'c' && buf[1] == 'o' && buf[2] == 'l') {
        print_mez("$*");
        print_mezc(buf[3]);
    }
    else if (buf[0] == 'm' && buf[1] == 'e' && buf[2] == 'z') {
        filedesc_t fd = open(buf + 4);
        if (fd) {
            uint32_t count;
            for(;;) {
                count = read(fd, buf, 64);
                if (!count) {
                    break;
                }
                print_mez(buf);
            }
            print("\n");
            close(fd);
        }
        else {
            print("file not found\n");
        }
    }
    else if (buf[0] == 'p' && buf[1] == 'u' && buf[2] == 't') {
        printf("%s", buf + 4);
    }
    else if (buf[0] == 'w' && buf[1] == 'a' && buf[2] == 'i' && buf[3] == 't') {
        time_t t1, t2;
        struct time_spec ts;
        clock_gettime(&ts);
        t1 = ts.sec * 1000000000 + ts.nsec;
        t2 = t1;
        time_t c = 1000000 * (time_t)atoi(buf + 4);
        while (t2 - t1 < c) {
            clock_gettime(&ts);
            t2 = ts.sec * 1000000000 + ts.nsec;
        }
    }
    else if (buf[0] == 'l' && buf[1] == 's') {
        readdir(buf + 3, lsfn);
    }
    else if (!strcmp(buf, "sleep")) {
        int t = 0;
        for(;;) {
            if (read(0, (byte_t[]) {0}, 1)) {
                break;
            }
            lseek(screen, 0, 0);
            for (int y = 0; y < 480; ++y) {
                byte_t line[640];
                for (int x = 0; x < 640; ++x) {
                    int T = t / 16;
                    line[x] = (((x & y) + x + y + T) % 64) < 32 ? 15 : 0;
                }
                write(screen, line, 640);
            }
            ++t;

            for (int i = 0; i < 10000; ++i) asm("pause");
        }
        print("\x80");
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
            char m2_prev = 0;
            uint8_t col = 0x0F;
            while (!read(0, (byte_t[]) {0}, 1)) {
                int mouse[3] = {0};
                ioctl(3, 50, mouse, 0, 0);
                if (mouse[2] & 1) {
                    lseek(screen, mouse[0]+mouse[1]*640, 0);
                    write(screen, &col, 1);
                }
                if (mouse[2] & 4) {
                    lseek(screen, mouse[0]+mouse[1]*640, 0);
                    read(screen, &col, 1);
                }
                if (mouse[2] & 2) {
                    if (!m2_prev) {
                        m2_prev = 1;
                        col = (col + 1) % 0x10;
                        lseek(screen, 0, 0);
                        write(screen, &col, 1);
                    }
                }
                else {
                    m2_prev = 0;
                }
            }
        }
    }
    else if (!strcmp(buf, "uexec")) {
        char buf[64];
        for (int i = 0; i < 64; ++i) {
            buf[i] = 0;
        }
        while (!read(0, buf, 64));
        shell(buf);
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
    else if (!strcmp(buf, "sound")) {
        char c;
        int file = open("/dev/spc");
        for (;;) {
            if (read(0, &c, 1)) {
                write(file, (byte_t*)((uint16_t[]) {c, 20}), 4);
            }
        }
        close(file);
    }
}

// NOTE: remove this before push
// #define DEBUG

void _start() {
    if (is_bosy()) {
        screen = open("/dev/screen");
        printf("screen: %d\n", screen);
    }
    #ifdef DEBUG
    print("Testing\n");
    shell("bsh test.bsh");
    print("\nSCRIPT END.\n");
    #else
    printf("Loading /etc/rc.bsh\n");
    shell("bsh etc/rc.bsh");
    #endif
    for(;;);
}