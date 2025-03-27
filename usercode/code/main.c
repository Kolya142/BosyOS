#include "bosyos/bosystd.h"
#include "mezdoc.h"
#include <stdarg.h>

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

void sleep(time_t ms) {
    struct time_spec ts;
    clock_gettime(&ts);

    time_t start = ts.sec * 1000 + ts.nsec / 1000000;
    time_t now = start;

    while (now - start < ms) {
        clock_gettime(&ts);
        now = ts.sec * 1000 + ts.nsec / 1000000;
    }
}

char *ccc;

#define QUEUE_SIZE 64
char queue[QUEUE_SIZE][128];
int q_head = 0, q_tail = 0, q_count = 0;
char text_buf[80*60];

void queue_add(const char *cmd) {
    if (q_count >= QUEUE_SIZE) {
        return;
    }
    ++q_count;
    for (int i = 0; i < 128 && cmd[i]; ++i) {
        queue[q_tail][i] = cmd[i];
    }
    q_tail = (q_tail + 1) % QUEUE_SIZE;
}
int queue_get(char *cmd) {
    if (q_count == 0) {
        return 0;
    }
    --q_count;
    for (int i = 0; i < 128; ++i) {
        cmd[i] = queue[q_head][i];
    }
    q_head = (q_head + 1) % QUEUE_SIZE;
    return 1;
}

struct utsname {
    char sysname[65];
    char nodename[65];
    char release[65];
    char version[65];
    char machine[65];
};
struct button {
    int x, y;
    int len;
    char text[64];
    char action[64];
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

int mousex, mousey, mousetx, mousety;

int min(int a, int b) {
    return a < b ? a : b;
}

void shell(char *buf) {
    int s = strlen(buf);
    if (buf[s-1] == '\n') {
        buf[s-1] = 0;
    }
    
    if (buf[0] == 'b' && buf[1] == 's' && buf[2] == 'h') {
        int file = open(buf + 4);
        // printf("bsh: %d\n", file);
        if (file) {
            char fbuf[2048] = {0};
            read(file, fbuf, 2048);
            // printf("script: \n%s\n", fbuf);
            int i = 0;
            int j = 0;
            char line[128] = {0};
            for (;fbuf[i];++i) {
                if (fbuf[i] == '\n') {
                    line[j] = 0;
                    j = 0;
                    // printf("line: %s\n", line);
                    if (line[0] == '#');
                    else {
                        queue_add(line);
                    }
                }
                else if (j < sizeof(line) - 1) {
                    line[j++] = fbuf[i];
                }
            }
            if (j > 0) {
                line[j] = 0;
                queue_add(line);
            }
            close(file);
        }
        else {
            printf("Could not open file %s\n", buf + 4);
        }
    }
    else if (!strcmp(buf, "help")) {
        print_mez(
            "$!Absh$!F   - execute bsh script\n"
            "$!Ahelp$!F  - show this text\n"
            "$!Aex$!F    - run EXpression\n"
            "$!Acll$!F   - set pallete\n"
            "$!Aecho$!F  - show text\n"
            "$!Aechm$!F  - show text in mezex without \\n\n"
            "$!Asleep$!F - enter simple sleep mode animation\n"
            "$!Aed$!F    - UNIX-like editor\n"
            "$!Apaint$!F - paint\n"
            "$!Asong$!F  - play random song\n"
        );
    }
    else if (!strcmp(buf, "song")) {
        int speaker = open("/dev/spc");
        int rand = open("/dev/urandom");
        int temp[256] = {0};
        int tempi = 0;
        int tone = 0x50;
        int fuzz = 0;
        int r = 0;
        int t = 0;
        if (speaker) {
            for (;tempi < sizeof(temp) / 4;) {
                read(rand, (byte_t*)&r, 4);
                tone += (rand % 40) - 20;
                fuzz += (rand % 40) - 20;
                tone = (tone * 7 + fuzz) / 8;
                if (tone < 20) {
                    tone = 20;
                }
                if (tone > 60) {
                    tone = 60;
                }
                {
                    short t[] = {tone, (rand % 30) + 50};
                    temp[tempi++] = (t[0] << 16) | t[1];
                }
                if (!((rand + t) % 6) && (tempi+1) < sizeof(temp) / 4) {
                    read(rand, (byte_t*)&r, 4);
                    {
                        short t[] = {tone += 10, (rand % 30) + 50};
                        temp[tempi++] = (t[0] << 16) | t[1];
                    }
                }
                if (((rand + t) % 10 < 3) && (tempi+1) < sizeof(temp) / 4) {
                    read(rand, (byte_t*)&r, 4);
                    {
                        short t[] = {tone -= 10, (rand % 30) + 50};
                        temp[tempi++] = (t[0] << 16) | t[1];
                    }
                }
                if (((rand + t) % 10 < 7) && (tempi+1) < sizeof(temp) / 4) {
                    read(rand, (byte_t*)&r, 4);
                    {
                        short t[] = {tone += 5, (rand % 30) + 50};
                        temp[tempi++] = (t[0] << 16) | t[1];
                    }
                }
                ++t;
            }
            for (int i = 0; i < sizeof(temp) / 4; ++i) {
                int tone = temp[i] >> 16;
                read(rand, (byte_t*)&r, 4);
                printf("%c%c ", 'A' + (tone / 4), ".,#%"[tone % 4]);
                printf("%d %d\n", temp[i] & 0xFFFF);
                write(speaker, (byte_t*)&temp[i], 4);
                sleep((r % 50) + 50);
            }
            close(speaker);
        }
        close(rand);
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
    else if (buf[0] == 'e' && buf[1] == 'x') {
        char *ibuf = buf + 3;
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
    else if (buf[0] == 'c' && buf[1] == 'l' && buf[2] == 'l') {
        printf("%s\n", buf);
        uint32_t ibuf[16], tmp;
        for (int i = 0; i < 16; ++i) {
            tmp = 0;
            for (int j = 0; j < 6; ++j) {
                int c = buf[3 + i * 6 + j];
                tmp <<= 4;
                if (c >= '0' && c <= '9') {
                    tmp |= c - '0';
                }
                else if (c >= 'A' && c <= 'F') {
                    tmp |= c - 'A' + 10;
                }
                else if (c >= 'a' && c <= 'f') {
                    tmp |= c - 'a' + 10;
                }
                // printf("%c ", c);
            }
            // printf("\n");
            ibuf[i] = tmp;
        }
        int file = open("/dev/color");
        write(file, (byte_t*)ibuf, sizeof(ibuf));
        close(file);
    }
    else if (buf[0] == 'e' && buf[1] == 'c' && buf[2] == 'h' && buf[3] == 'o') {
        printf("%s\n", buf + 5);
    }
    else if (buf[0] == 'e' && buf[1] == 'c' && buf[2] == 'h' && buf[3] == 'm') {
        print_mez(buf + 5);
    }
    else if (buf[0] == 't' && buf[1] == 'h') {
        syscall(8, (uint32_t)(buf + 3), 0, 0, 0, 0, 0);
    }
    else if (!strcmp(buf, "cls") || !strcmp(buf, "clear")) {
        if (is_bosy()) {
            print("\x80");
        }
        else {
            print("\x1b[2J\x1b[H");
        }
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
                    line[x] = (((x & y) + x + y + T) % 256) / 16;
                }
                write(screen, line, 640);
            }
            ++t;

            for (int i = 0; i < 10000; ++i) asm("pause");
        }
        print("\x80");
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
}

// NOTE: remove this before push
// #define DEBUG

void _start() {
    if (is_bosy()) {
        screen = open("/dev/screen");
        printf("screen: %d\n", screen);
    }
    else {
        printf("sorry, your system is't bosyos");
    }
    lseek(1, 0, 0);
    print_mez("$!A\\$$!F ");
    int last = 0;
    char buf[128];
    shell("bsh /etc/rc.bsh");

    for(;;) {
        memset(buf, 0, sizeof(buf));
        if (queue_get(buf)) {
            shell(buf);
            last = 0;
            continue;
        }
        else {
            if (!last) {
                last = 1;
                print_mez("$!A\\$$!F ");
            }
            if (read(0, buf, 128)) {
                queue_add(buf);
            }
        }
        sleep(1000/60);
    }
}