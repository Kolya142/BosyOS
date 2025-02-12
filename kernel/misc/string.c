#include <misc/string.h>

Char ToUpper(Char a)
{
    if (a == 0x30) {
        return ')';
    }
    if (a >= 0x30 && a <= 0x39) {
        return ")!@#$%^&*("[a-'0'];
    }
    if (a == '-') {
        return '_';
    }
    if (a == '=') {
        return '+';
    }
    if (a == '`') {
        return '~';
    }
    if (a >= 'a' && a <= 'z') {
        return a+'A'-'a';
    }
    return a;
}
U32 StrLen(const Char *str) {
    U32 len = 0;
    while (str[len])
        len++;
    return len;
}

U32 StrCmp(const Char *a, const Char *b)
{
    U32 p1 = 0, p2 = 0;
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
Char StrStartsWith(const Char *a, const Char *b) {
    for (U32 i = 0; b[i]; i++) {
        if (!a[i] || a[i] != b[i]) {
            return 0;
        }
    }
    return 1;
}