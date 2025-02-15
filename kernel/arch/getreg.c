#include <arch/getreg.h>

U32 RegGet(U8 reg)
{
    U32 res;
    switch (reg) {
        case REG_EAX:
            asmV("mov %%eax, %0": "=r"(res) :: "eax");
        break;
        case REG_EBX:
            asmV("mov %%ebx, %0": "=r"(res) :: "ebx");
        break;
        case REG_ECX:
            asmV("mov %%ecx, %0": "=r"(res) :: "ecx");
        break;
        case REG_EDX:
            asmV("mov %%edx, %0": "=r"(res) :: "edx");
        break;
        case REG_ESI:
            asmV("mov %%esi, %0": "=r"(res) :: "esi");
        break;
        case REG_EDI:
            asmV("mov %%edi, %0": "=r"(res) :: "edi");
        break;
    }
    return res;
}