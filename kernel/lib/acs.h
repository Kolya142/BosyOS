#include <kernel/alloc.h>
#include <lib/string.h>
#pragma once
#define alloc(T, V) (T*)HeapAlloc(V)

int acs_alog(int a, int b);

char *acs_add(const char *a, const char *b);
char *acs_substr(const char *a, int start, int end);
void acs_creplace(char *v, char a, char b);
char *acs_printint(int a);
int acs_parseint(const char *a);
char *acs_cp(const char *a);
typedef struct acs_spliter {
    char *s;
    int sz;
    int p;
    int end;
} acs_spliter;
acs_spliter *acs_spliter_init(const char *s);
void acs_spliter_upd(acs_spliter *self, const char *s);
unsigned char acs_spliter_check_result(acs_spliter *self);
void acs_spliter_free(acs_spliter *self);
char *acs_spliter_char(acs_spliter *self, char split);
char *acs_spliter_string(acs_spliter *self, const char *split);
char *acs_milloc(char *s);