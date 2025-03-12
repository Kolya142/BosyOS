#pragma once
#include <lib/Types.h>
#include <lib/memory/MemLib.h>
Char UpperTo(Char a);
String Utf8ToCP517(const String utf8);
U32 StrLen(const Char *str);
U0 StrCpy(String dst, String src);
U32 StrCmp(const Char *a, const Char *b);
U32 Atoi(String a);
Char StrStartsWith(const Char *a, const Char *b);