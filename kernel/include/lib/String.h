#pragma once
#include <lib/MemLib.h>
#include <lib/Types.h>
Char UpperTo(Char a);
String Utf8ToCP517(const String utf8);
U32 StrLen(const Char *str);
U0 StrCpy(String dst, String src);
U32 StrCmp(const Char *a, const Char *b);
Char StrStartsWith(const Char *a, const Char *b);