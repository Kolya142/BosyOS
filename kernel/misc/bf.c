#include <misc/bf.h>
#include <kernel/KMem.h>
#include <libs/tty.h>

typedef enum CommandType {
    LEFT      = '<',
    RIGHT     = '>',
    JUMPLEFT  = '[',
    JUMPRIGHT = ']',
    PLUS      = '+',
    MINUS     = '-',
    OUTPUT    = '.',
    OUTPUT2   = '/',
    INPUT     = ',',
    DEBUG     = '}',
} CommandType;

typedef struct Command {
    CommandType type;
    int count;
} Command;

typedef struct CommandList {
    Command *list;
    int count;
    int avaliable;
} CommandList;

#define malloc HeapAlloc

void *_realloc(void *arr, int old, int new) {
    void *temp = malloc(new);
    MemCpy(temp, arr, old);
    HeapFreePtr(arr);
    return temp;
}

#define realloc(arr, old, new) _realloc(arr, old, new)

#define al_append(arr, el) { \
    if (1+(arr).count > (arr).avaliable) { \
        (arr).list = realloc(((arr).list), (arr).avaliable, ((arr).avaliable<<=1) * sizeof(el)); \
    }  \
    (arr).list[(arr).count] = (el);  \
    (arr).count++; \
    } 

void BFInterpret(const char *prog, unsigned char *mem, const char *input, int *Stack, int *stackptr, int inputl, int *inputp) {
    int *S = Stack;
    char *l = (char*)prog;
    int L = *stackptr;
    unsigned char *m = mem;
    CommandList cl = (CommandList){
        .list      = malloc(sizeof(Command)*8),
        .count     = 0,
        .avaliable = 8,
    };
    char last = '&';
    int clen = 0;
    while(*l) {
        if (*l != '<' && *l != '>' && 
            *l != '+' && *l != '-' &&
            *l != '.' && *l != ',' &&
            *l != '/' && *l != '[' &&
            *l != ']') {
            l++;
            continue;
        }
        switch(*l) {
            case '<': case '>': case '+': case '-': case '.': case ',': case '/': case '}': {
                if (last == *l) {
                    clen++;
                    cl.list[cl.count-1].count++;
                }
                else {
                    clen = 1;
                    Command cmd = (Command) {
                        .type = *l,
                        .count = clen,
                    };
                    al_append(cl, cmd);
                }
            }
            break;
            case '[': {
                S[L]=cl.count;
                L++;
                Command cmd = (Command) {
                    .type = '[',
                    .count = 0,
                };
                al_append(cl, cmd);
                }
                break;
            case ']': {
                int i = S[L-1];
                cl.list[i].count = cl.count;
                L--;
                Command cmd = (Command) {
                    .type = ']',
                    .count = i,
                };
                al_append(cl, cmd);
                }
                break;
            }
            last = *l;
            l++;
    }
    int i = 0, IP = 0, *ip = &IP;
    while (i < cl.count) {
        Command el = cl.list[i];
        // putchar(el.type);
        switch (el.type) {
            case '<': {
                *ip -= el.count;
                i++;
            } break;
            case '>': {
                *ip += el.count;
                i++;
            } break;
            case '+': {
                m[*ip] += el.count;
                i++;
            } break;
            case '-': {
                m[*ip] -= el.count;
                i++;
            } break;
            case '.': {
                TTYPrintC(m[*ip]);
                i++;
            } break;
            case '}': {
                i++;
            } break;
            case ',': {
                if (*inputp < inputl) {
                    m[*ip] = input[*inputp];
                    *inputp++;
                }
                i++;
            } break;
            case '/': {
                TTYPrintC("0123456789ABCDEF"[m[*ip]>>4]);
                TTYPrintC("0123456789ABCDEF"[m[*ip]&15]);
                i++;
            }break;
            case '[': {
                if (!m[*ip]) {i = el.count+1;}
                else {i++;}
            }break;
            case ']': {
                if (m[*ip]) {i = el.count+1;}
                else {i++;}
            }break;
        }
    }
    *stackptr = L;
}