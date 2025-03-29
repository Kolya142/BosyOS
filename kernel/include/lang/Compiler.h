#pragma once
#include <lib/memory/Collects.h>
#include <lang/Tokenizer.h>
#include <lib/Types.h>

#define NEXTTOK {do { a = TokenNext(code, &tok); a = tok.type == TOK_STR ? a + 2 : a; code += a; sym += a;} while (!*tok.str && a);}

extern List CompilerRoData;

// Read more: https://github.com/cia-foundation/TempleOS/blob/archive/Compiler/OpCodes.DD#L215
typedef enum ASMInst {
    ASM_ADD  = 0x01, // R/M R
    ASM_OR   = 0x09, // R/M R
    ASM_AND  = 0x21, // R/M R
    ASM_SUB  = 0x29, // R/M R
    ASM_XOR  = 0x31, // R/M R
    ASM_CMP  = 0x39, // R/M R

    ASM_MOV_R2R = 0x89, // R32 RM R32
    ASM_MOV_RR  = 0x8B, // R32 R/M32
    ASM_MOV_IMM = 0xB8, // R32 IMM32

    ASM_IMUL = 0xAF, // R32 R/M32
    ASM_NOT  = 0xF7, // R/M32
    ASM_IDIV = 0xF7, // EDX R/M32

    ASM_IN   = 0xE4, // AL IMM8
    ASM_OUT  = 0xE6, // IMM8 AL

    ASM_JE   = 0x84,
    ASM_JNE  = 0x85,
    ASM_JBE  = 0x86,
    ASM_JP   = 0x8A,
    ASM_JNP  = 0x8B,
    ASM_JL   = 0x8C,
    ASM_JLE  = 0x8E,
    ASM_JMP  = 0xE9
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
U0 ASMInstMake32(Bool stmode, U8 uses, U8 inst, U8 modrm, U8 sib, U32 disp, U32 imm);

U0 CompilerEmit(U8 code);

U0 CompilerInit();
List Compiler(String code);

U32 CompilerExpr(String code);

String RegName(U8 reg);
U8 RegFromName(String name);

U0 ASMDis(U8* code, U32 count);

#define ASMInstJeIMM32(offset)  ASMInstJccIMM32(ASM_JE,  offset)
#define ASMInstJneIMM32(offset) ASMInstJccIMM32(ASM_JNE, offset)
#define ASMInstJbeIMM32(offset) ASMInstJccIMM32(ASM_JBE, offset)
#define ASMInstJpIMM32(offset)  ASMInstJccIMM32(ASM_JP,  offset)
#define ASMInstJnpIMM32(offset) ASMInstJccIMM32(ASM_JNP, offset)
#define ASMInstJlIMM32(offset)  ASMInstJccIMM32(ASM_JL,  offset)
#define ASMInstJleIMM32(offset) ASMInstJccIMM32(ASM_JLE, offset)

U0 ASMInstMovReg2Reg32(U8 dst, U8 src);
U0 ASMInstMovIMM2Reg32(U8 dst, U32 imm);
U0 ASMInstMovReg2Mem32(U8 dst, U8 src);
U0 ASMInstMovMem2Reg32(U8 dst, U8 src);

U0 ASMInstAddReg2Reg32(U8 dst, U8 src);
U0 ASMInstIMulReg2Reg32(U8 dst, U8 src);
U0 ASMInstJccIMM32(U8 opcode, I32 offset);
U0 ASMInstJmpIMM32(I32 offset);
U0 ASMInstSubReg2Reg32(U8 dst, U8 src);
U0 ASMInstXorReg2Reg32(U8 dst, U8 src);
U0 ASMInstOrReg2Reg32(U8 dst, U8 src);
U0 ASMInstAndReg2Reg32(U8 dst, U8 src);
U0 ASMInstNotReg2Reg32(U8 reg);
U0 ASMInstCmpReg2Reg32(U8 r1, U8 r2);
U0 ASMInstCmpImm2Reg32(U8 reg, U32 imm);
U0 ASMInstCallIMM32(I32 offset);
U0 ASMInstCallReg32(U8 reg);