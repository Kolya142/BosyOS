#include "string.h"

char ToUpper(char a)
{
    if (a == 0x30) {
        return ')';
    }
    if (a >= 0x31 && a <= 0x39) {
        return a+'!'-'1';
    }
    if (a >= 'a' && a <= 'z') {
        return a+'A'-'a';
    }
    return a;
}
int StrLen(const char *str) {
    int len = 0;
    while (str[len])
        len++;
    return len;
}

int StrCmp(const char *a, const char *b)
{
    int p1 = 0, p2 = 0;
    if ((!*a && *b) || (*a && !*b)) {
        return -1;
    }
    while (a[p1] && b[p2]) {
        if (a[p1] != b[p2]) {
            return a[p1]-b[p2];
        }
        p1++;
        p2++;
    }
    return p1-p2;
}
char StrStartsWith(const char *a, const char *b) {
    for (int i = 0; b[i]; i++) {
        if (!a[i] || a[i] != b[i]) {
            return 0;
        }
    }
    return 1;
}
