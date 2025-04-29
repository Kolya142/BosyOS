#include <lib/String.h>


U32 StrCmp(const Char *a, const Char *b) {
    while (*b && (*a == *b)) {++a; ++b;}

    return (U32)(*a-*b);
}
String StrDup(String s) {
    U32 size = StrLen(s) + 1;
    String new = KMAlloc(size);
    MemCpy(new, s, size);
    return new;
}
U32 StrLen(String s) {
    U32 len = 0;
    while (s[len]) len++;
    return len;
}
Char Upper(Char c) {
    if (c >= 0x30 && c <= 0x39) {
        return ")!@#$%^&*("[c-'0'];
    }
    if (c == '-') return '_';
    if (c == '=') return '+';
    if (c == '`') return '~';
    if (c == ',') return '<';
    if (c == '.') return '>';
    if (c == '[') return '{';
    if (c == ']') return '}';
    if (c == '/') return '?';
    if (c == '\'') return '"';
    if (c == ';') return ':';
    if (c >= 'a' && c <= 'z') {
        return c-'a'+'A';
    }
    return c;
}
U32 Atoi(String a) {
    U32 v = 0;
    U8 mode = 0;
    if (a[0] == '0' && a[1] == 'x') {
        mode = 1;
        a += 2;
    }
    while (*a) {
        if (mode == 0) {
            if (*a >= '0' && *a <= '9') {
                U8 n = *a-'0';
                v = (v * 10) + n;
            }
        }
        else if (mode == 1) {
            if (*a >= '0' && *a <= '9') {
                U8 n = *a-'0';
                v = (v * 16) + n;
            }
            else if (*a >= 'A' && *a <= 'F') {
                U8 n = *a-'A';
                v = (v * 16) + n + 10;
            }
            else if (*a >= 'a' && *a <= 'f') {
                U8 n = *a-'a';
                v = (v * 16) + n + 10;
            }
        }
        ++a;
    }
    return v;
}