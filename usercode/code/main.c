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

// NOTE: remove this before push
// #define DEBUG

void _start() {
    // if (is_bosy()) {
    //     screen = open("/dev/screen");
    //     printf("screen: %d\n", screen);
    // }
    // else {
    //     printf("sorry, your system is't bosyos");
    // }
    // lseek(1, 0, 0);
    // print_mez("$!A\\$$!F ");
    // int last = 0;
    // char buf[128];
    // shell("bsh /etc/rc.bsh");

    // for(;;) {
    //     memset(buf, 0, sizeof(buf));
    //     if (queue_get(buf)) {
    //         shell(buf);
    //         last = 0;
    //         continue;
    //     }
    //     else {
    //         if (!last) {
    //             last = 1;
    //             print_mez("$!A\\$$!F ");
    //         }
    //         if (read(0, buf, 128)) {
    //             queue_add(buf);
    //         }
    //     }
    //     sleep(1000/60);
    // }
    for(;;);
}