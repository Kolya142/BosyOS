#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>

void TTYUPrintC(char c) {
    static bool lbs = false;
    static bool lds = false; // $
    static char   ldv = 0    ;
    static char   lbd = 0    ;
    static char   lbp = 0    ;
    static const unsigned char fgs[] = {
        30, 34, 32, 36, 31, 35, 33, 90,
        37, 94, 92, 96, 91, 95, 93, 97
    };
    static const unsigned char bgs[] = {
        40, 44, 42, 46, 41, 45, 43, 100,
        47, 104, 102, 106, 101, 105, 103, 107
    };
    if (lbs) {
        if (c == '\\') 
            putchar('\\');
        if (c == 'n')
            putchar('\n');
        if (c == '$') 
            putchar('$'); 
        lbs = false;
    }
    else if (lds) {
        if (c == '!') {
            ldv = 1;
        }
        else if (c == '*') {
            ldv = 2;
        }
        else if (c == 'x') {
            lbp = 1;
        }
        else if (lbp) {
            lbd <<= 4;
            unsigned char d = 255;
            if (c >= '0' && c <= '9') {
                d = c - '0';
            }
            else if (c >= 'A' && c <= 'F') {
                d = c - 'A' + 10;
            }
            else if (c >= 'a' && c <= 'f') {
                d = c - 'a' + 10;
            }
            lbd |= d;
            lbp ++;
            if (lbp == 3) {
                putchar(lbd);
                lbd = 0;
                lds = false;
                lbp = 0;
            }
        }
        else {
            unsigned char d = 255;
            if (c >= '0' && c <= '9') {
                d = c - '0';
            }
            else if (c >= 'A' && c <= 'F') {
                d = c - 'A' + 10;
            }
            else if (c >= 'a' && c <= 'f') {
                d = c - 'a' + 10;
            }
            if (d - 255) {
                switch (ldv) {
                    case 1:
                        printf("\x1b[%dm", fgs[d]);
                    break;
                    case 2:
                        printf("\x1b[%dm", bgs[d]);
                    break;
                }
            }
            ldv = 0;
            lds = false;
        }
    }
    else if (c & 0x80) {
        putchar(c);
    }
    else if (c == '\\') {lbs = true;}
    else {
        if (c == '$') {
            lds = true;
        } 
        else {
            putchar(c);
        }
    }
}

int main() {
    char c;
    while (read(0, &c, 1)) {
        TTYUPrintC(c);
    }
}