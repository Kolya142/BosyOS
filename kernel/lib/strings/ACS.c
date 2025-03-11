#include <lib/strings/ACS.h>

ACSSpliter ACSSpliterInit(String s) {
    return (ACSSpliter) {
        .s = s,
        .isend = False,
        .p = 0,
        .sz = StrLen(s)
    };
}
U0 ACSSpliterChar(ACSSpliter *self, Char split, String buf) {
    
}