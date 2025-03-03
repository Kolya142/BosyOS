#include <lib/String.h>

Char UpperTo(Char a)
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
    if (a == ',') {
        return '<';
    }
    if (a == '.') {
        return '>';
    }
    if (a == '[') {
        return '{';
    }
    if (a == ']') {
        return '}';
    }
    if (a == '/') {
        return '?';
    }
    if (a == '\'') {
        return '"';
    }
    if (a == ';') {
        return ':';
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
U0 StrCpy(String dst, String src) {
    MemCpy(dst, src, StrLen(src));
}

U32 StrCmp(const Char *a, const Char *b)
{
    while (*b && (*a == *b)) {++a; ++b;}

    return (U32)(*a-*b);
}
Char StrStartsWith(const Char *a, const Char *b) {
    for (U32 i = 0; b[i]; i++) {
        if (!a[i] || a[i] != b[i]) {
            return 0;
        }
    }
    return 1;
}