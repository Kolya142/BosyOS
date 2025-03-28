#pragma once
#include <lib/memory/Collects.h>
#include <lang/Tokenizer.h>
#include <lib/Types.h>

#define NEXTTOK {    do { a = TokenNext(code, &tok); code += a; sym += a;} while (!*tok.str && a);}

typedef enum ASMInst {
    ASM_ADD  = 0x00,
    ASM_OR   = 0x08,
    ASM_AND  = 0x20,
    ASM_MOV  = 0x22,
    ASM_SUB  = 0x28,
    ASM_XOR  = 0x30,
    ASM_CMP  = 0x3C,
    ASM_IMUL = 0xAF,
    ASM_IN   = 0xE4,
    ASM_OUT  = 0xE6,
    ASM_NOT  = 0xF6,
    ASM_IDIV = 0xF7,
} ASMInst;

#define ASM_REG_AL  0
#define ASM_REG_AX  0
#define ASM_REG_EAX 0
#define ASM_REG_CL  1
#define ASM_REG_CX  1
#define ASM_REG_ECX 1
#define ASM_REG_DL  2
#define ASM_REG_DX  2
#define ASM_REG_EDX 2
#define ASM_REG_BL  3
#define ASM_REG_BX  3
#define ASM_REG_EBX 3
#define ASM_REG_AH  4
#define ASM_REG_SP  4
#define ASM_REG_ESP 4
#define ASM_REG_CH  5
#define ASM_REG_BP  5
#define ASM_REG_EBP 5
#define ASM_REG_DH  6
#define ASM_REG_SI  6
#define ASM_REG_ESI 6
#define ASM_REG_BH  7
#define ASM_REG_DI  7
#define ASM_REG_EDI 7

typedef struct CompilerFunction {
    Char name[32];
    List code;
} CompilerFunction;

// (1<<0) - modrm, (1<<1) - sib, (1<<2) - disp, (1<<3) - imm
U0 ASMInstMake32(Bool stmode, U8 uses, U8 inst, U8 pref, U8 modrm, U8 sib, U32 disp, U32 imm);

U0 CompilerEmit(U8 code);

U0 CompilerInit();

List Compiler(String code);



U0 ASMInstMovReg2Reg32(U8 dst, U8 src);
U0 ASMInstMovIMM2Reg32(U8 dst, U32 imm);
U0 ASMInstAddReg2Reg32(U8 dst, U8 src);
U0 ASMInstXorReg2Reg32(U8 dst, U8 src);
U0 ASMInstOrReg2Reg32(U8 dst, U8 src);
U0 ASMInstAndReg2Reg32(U8 dst, U8 src);
U0 ASMInstNotReg2Reg32(U8 reg);
U0 ASMInstCmpReg2Reg32(U8 r1, U8 r2);
U0 ASMInstCmpImm2Reg32(U8 reg, U32 imm);
U0 ASMInstCallIMM32(I32 offset);
U0 ASMInstCallReg32(U8 reg);