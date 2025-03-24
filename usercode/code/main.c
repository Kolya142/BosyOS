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

int atoi(const char *str) {
    int n = 0;
    while (*str) {
        if (*str >= '0' && *str <= '9') {
            n += *str - '0';
            n *= 10;
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
            "fetch - show simple system information\n"
            "paint - just paint\n"
            "put   - print text without \\n\n"
            "uexec - read and exec user's command\n"
            "anim  - show a simple animation\n"
            "cls   - clear screen\n"
            "clear - clear screen\n"
            "tut   - open a simple tutorial\n"
            "ls    - list files\n"
            "cat   - read file\n"
            "wait  - wait for X millis\n"
            "sleep - enter sleep mode\n"
            "@WORD - generate acronym for WORD\n"
            "!FILE - run FILE as elf executable\n"
            "bsh   - load bsh script\n"
            "echo  - print text\n"
        );
    }
    else if (buf[0] == 'e' && buf[1] == 'c' && buf[2] == 'h' && buf[3] == 'o') {
        printf("%s\n", buf + 5);
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
    else if (buf[0] == '@') {
        char *dict[127][6] = {
            ['A'] = {"Advanced", "Anomalous","Atomic",     "A",         0,      0},
            ['B'] = {"Boot",     "Binary",   "Bosonic",    "Bionic",    "Busy", 0},
            ['C'] = {"Core",     "Cyber",    "Chaos",      "Cat",       0,      0},
            ['D'] = {"Daemon",   "Dark",     "Direct",     "Do",        0,      0},
            ['E'] = {"Echo",     "Engine",   "Error",      0,           0,      0},
            ['F'] = {"File",     "Fork",     "False",      0,           0,      0},
            ['G'] = {"Graph",    "Ghost",    "Grid",       0,           0,      0},
            ['H'] = {"Hell",     "Heap",     "Hack",       0,           0,      0},
            ['I'] = {"Interrupt","Infinite", "Idle",       0,           0,      0},
            ['J'] = {"Jump",     "Joke",     "Jitter",     0,           0,      0},
            ['K'] = {"Kernel",   "Key",      "Khaos",      0,           0,      0},
            ['L'] = {"Lazy",     "Logic",    "Link",       0,           0,      0},
            ['M'] = {"Module",   "Mist",     "Meta",       0,           0,      0},
            ['N'] = {"Null",     "Number",   "Noise",      0,           0,      0},
            ['O'] = {"Onion",    "Output",   "Overflow",   0,           0,      0},
            ['P'] = {"Pointer",  "Port",     "Panic",      0,           0,      0},
            ['Q'] = {"Quantum",  "Queue",    "Quit",       0,           0,      0},
            ['R'] = {"Runtime",  "Reset",    "Recursive",  0,           0,      0},
            ['S'] = {"System",   "Stack",    "Soup",       0,           0,      0},
            ['T'] = {"Thread",   "Trap",     "Tear",       "Tar",       0,      0},
            ['U'] = {"Unix",     "Unknown",  "User",       "Utmost",    "Up",   0},
            ['V'] = {"Void",     "Virtual",  "Verbose",    "Valley",    "view", 0},
            ['W'] = {"Write",    "Wave",     "Warp",       "Wonderful", 0,      0},
            ['X'] = {"Xor",      "Xeno",     "eXecute",    "Xen",       0,      0},
            ['Y'] = {"Yellow",   "Yield",    "Yet",        "Yes",       "Yon",  0},
            ['Z'] = {"Zone",     "Zero",     "Zalgo",      0,           0,      0},
        };
        ++buf;
        int rand = open("/dev/urandom");
        for (;*buf;++buf) {
            int len = 0;
            for (;dict[*buf][len];++len);
            int i;
            do {
                read(rand, (byte_t*)&i, sizeof(i));
            }
            while (!dict[*buf][i%len]);
            printf("%s ", dict[*buf][i%len]);
        }
        close(rand);
        print("\n");
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
    else if (buf[0] == 'p' && buf[1] == 'u' && buf[2] == 't') {
        printf(buf + 4);
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
        print("\x80write something and press enter to exit\n");
        for(;;) {
            if (read(0, (byte_t[]) {0}, 1)) {
                break;
            }
            lseek(screen, 0, 0);
            for (int y = 0; y < 200; ++y) {
                byte_t line[320];
                for (int x = 0; x < 320; ++x) {
                    int T = t / 256;
                    line[x] = (((x & y) + T) % 8) < 4 ? 15 : 0;
                }
                write(screen, line, 320);
            }
            ++t;

            for (int i = 0; i < 10000; ++i) asm("pause");
        }
        print("\x80");
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
                    lseek(screen, mouse[0]+mouse[1]*320, 0);
                    write(screen, &col, 1);
                }
                if (mouse[2] & 4) {
                    lseek(screen, mouse[0]+mouse[1]*320, 0);
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
    const char *commands[] = {
        "bsh test.bsh"
    };
    for (int i = 0; i < sizeof(commands)/sizeof(commands[0]); ++i) {
        shell(commands[i]);
    }
    print("\nSCRIPT END.\n");
    for(;;);
    #else
    printf("Loading /etc/rc.bsh\n");
    shell("bsh etc/rc.bsh");
    #endif
}