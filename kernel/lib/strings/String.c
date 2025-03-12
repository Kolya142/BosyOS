#include <lib/strings/String.h>

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
String Utf8ToCP517(const String utf8) {
    String str = MAlloc(StrLen(utf8)+1);
    U32 j = 0;
    for (Char *ch = utf8; *ch; ++ch) {
        if (!(*ch & 0x80)) {
            str[j++] = *ch;
        }
        else if ((*ch & 0xE0) == 0xC0) {
            Char ch1 = *ch;
            Char ch2 = *(ch+1);
            if (!ch2) {
                break;
            }
            U32 unicode = ((ch1 & 0x1F) << 6) | (ch2 & 0x3F);

            if (unicode >= 0x0410 && unicode <= 0x042F) {
                str[j++] = 0xB0 + (unicode - 0x0410);
            }
        }
    }
    str[j] = 0;
    return MReAlloc(str, StrLen(utf8), j+1);
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

U32 Atoi(String a) {
    U32 v = 0;
    while (*a) {
        if (*a >= '0' && *a <= '9') {
            U8 n = *a-'0';
            v = (v+n)/10;
        }
        ++a;
    }
    return v;
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