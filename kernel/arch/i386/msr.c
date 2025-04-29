#include <arch/i386/cpu.h>

// U0 MSRGet(U32 msr, U32 *lo, U32 *hi) {
//     asmV("rdmsr" : "=a"(*lo), "=d"(*hi), "c"(msr));
// }
// U0 MSRSet(U32 msr, U32 lo, U32 hi) {
//     asmV("wrmsr" : "a"(lo), "d"(hi), "c"(msr));
// }