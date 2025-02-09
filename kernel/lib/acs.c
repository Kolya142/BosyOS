#include "acs.h"
int acs_alog(int a, int b) {
    int i = 0;
    while( a > 0 ){
        a /= b;
        i++;
    }
    return i;
}

char *acs_add(const char *a, const char *b) {
    int la = StrLen(a);
    int lb = StrLen(b);
    char *o = alloc(char, la+lb+1);
    for (int i = 0; i < la+lb; i++) {
        if( i < la ){
            o[i] = a[i];
            continue;
        }
        o[i] = b[i - la];
    }
    o[la+lb] = 0;
    return o;
}
char *acs_substr(const char *a, int s, int e) {
    /*;
       a        start       end;
       |          |          |;
       0  1   2   3  4   5   6;
       ;
       returnurns 3,4,5 from a;
    */
    int l = StrLen(a);
    if( s < 0 || e >= l ){
        return NULL;
    }
    char *b = alloc(char, e-s+1);
    MemCpy(b, a+s, e-s);
    b[e-s] = 0;
    return b;
}
void acs_creplace(char *v, char a, char b) {
    int s = StrLen(v);
    for (int i = 0; i < s; i++) {
        if( v[i] == a ){
            v[i] = b;
        }
    }
}
char *acs_printint(int a) {
    unsigned char n = 0;
    if( a < 0 ){
        n = 1;
        a = -a;
    }
    int S = acs_alog(a, 10);
    char *s = alloc(char, S);
    int p = 0;
    if( n ){
        s[0] = '-';
        p--;
    }
    while( a > 0 ){
        s[S-1-p] = 0x30 + a % 10;
        a /= 10;
        p++;
    }
    return s;
}
int acs_parseint(const char *a) {
    int v = 0;
    int i = 0;
    for (;;) {
        if( a[i] == 0 ){
            break;
        }
        if( a[i] >= 0x30 && a[i] <= 0x39 ){
            v *= 10;
            v += a[i] - 0x30;
        }
        i++;
    }
    return v;
}
char *acs_cp(const char *a) {
    int s = StrLen(a);
    char *o = alloc(char, s);
    MemCpy(o, a, s);
    return o;
}
acs_spliter *acs_spliter_init(const char *s) {
    acs_spliter *o = alloc(acs_spliter, 1);
    o->s = s;
    o->p = 0;
    o->sz = StrLen(s);
    o->end = 0;
    return o;
}
void acs_spliter_upd(acs_spliter *self, const char *s) {
    self->s = s;
    self->p = 0;
    self->sz = StrLen(s);
    self->end = 0;
}
unsigned char acs_spliter_check_result(acs_spliter *self) {
    return self->end;
}
void acs_spliter_free(acs_spliter *self) {
    HeapFreePtr(self->s);
    HeapFreePtr(self);
}
char *acs_spliter_char(acs_spliter *self, const char split) {
    char *o = alloc(char, self->sz);
    for (int i = 0; i < self->sz; i++) {
        o[i] = 0;
    }
    int i = self->p;
    for (;;) {
        if( i >= self->sz ){
            self->end = 1;
            return acs_milloc(o);
        }
        if( self->s[i] == split ){
            i++;
            break;
        }
        o[i-self->p] = self->s[i];
        i++;
    }
    self->p = i;
    return acs_milloc(o);
}
char *acs_spliter_string(acs_spliter *self, const char *split) {
    char *o = alloc(char, self->sz);
    int S = StrLen(split);
    for (int i = 0; i < self->sz; i++) {
        o[i] = 0;
    }
    int i = self->p;
    for (;;) {
        if( i >= self->sz ){
            self->end = 1;
            return acs_milloc(o);
        }
        if( self->s[i] == split[0] ){
            unsigned char res = 0;
            for (int j = 0; j < S; j++) {
                if( i+j > self->sz - 1 || self->s[i+j] != split[j] ){
                    res = 1;
                    break;
                }
            }
            if( !res ){
                i += S;
                break;
            }
        }
        o[i-self->p] = self->s[i];
        i++;
    }
    self->p = i;
    return acs_milloc(o);
}
char *acs_milloc(char *s) {
    char *o = acs_cp(s);
    HeapFreePtr(s);
    return o;
}