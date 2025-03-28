#include <lang/Compiler.h>

U0 ASMInstMake32(Bool stmode, U8 uses, U8 inst, U8 pref, U8 modrm, U8 sib, U32 disp, U32 imm) {
    if (stmode) {
        CompilerEmit(0x66);
    }
    CompilerEmit((inst << 2) | pref);
    if (uses & 1) {
        CompilerEmit(modrm);
    }
    if (uses & 2) {
        CompilerEmit(sib);
    }
    if (uses & 4) {
        CompilerEmit((disp >>  0) & 0xFF);
        CompilerEmit((disp >>  8) & 0xFF);
        CompilerEmit((disp >> 16) & 0xFF);
        CompilerEmit((disp >> 24) & 0xFF);
    }
    if (uses & 8) {
        CompilerEmit((imm >>  0) & 0xFF);
        CompilerEmit((imm >>  8) & 0xFF);
        CompilerEmit((imm >> 16) & 0xFF);
        CompilerEmit((imm >> 24) & 0xFF);
    }
}

U0 ASMInstMovReg2Reg32(U8 dst, U8 src) {
    ASMInstMake32(0, 1, ASM_MOV, 0b01, 0b11000000 | (src << 3) | dst, 0, 0, 0);
}
U0 ASMInstMovIMM2Reg32(U8 dst, U32 imm) {
    CompilerEmit(0xB8 + dst);
    CompilerEmit((imm >> 0) & 0xFF);
    CompilerEmit((imm >> 8) & 0xFF);
    CompilerEmit((imm >> 16) & 0xFF);
    CompilerEmit((imm >> 24) & 0xFF);
}


U0 ASMInstAddReg2Reg32(U8 dst, U8 src) {
    ASMInstMake32(0, 1, ASM_ADD, 0b01, 0b11000000 | (src << 3) | dst, 0, 0, 0);
}
U0 ASMInstXorReg2Reg32(U8 dst, U8 src) {
    ASMInstMake32(0, 1, ASM_XOR, 0b01, 0b11000000 | (src << 3) | dst, 0, 0, 0);
}
U0 ASMInstOrReg2Reg32(U8 dst, U8 src) {
    ASMInstMake32(0, 1, ASM_OR, 0b01, 0b11000000 | (src << 3) | dst, 0, 0, 0);
}
U0 ASMInstAndReg2Reg32(U8 dst, U8 src) {
    ASMInstMake32(0, 1, ASM_AND, 0b01, 0b11000000 | (src << 3) | dst, 0, 0, 0);
}
U0 ASMInstNotReg2Reg32(U8 reg) {
    ASMInstMake32(0, 1, ASM_NOT, 0b01, 0b11010000 | reg, 0, 0, 0);
}
U0 ASMInstCmpReg2Reg32(U8 r1, U8 r2) {
    ASMInstMake32(0, 1, 0x3B >> 2, 0b01, 0b11000000 | (r2 << 3) | r1, 0, 0, 0);
}
U0 ASMInstCmpImm2Reg32(U8 reg, U32 imm) {
    U8 modrm = (0b11 << 6) | (0b111 << 3) | reg;
    ASMInstMake32(0, 1 | 8, 0x81 >> 2, 0b11, modrm, 0, 0, imm);
}
U0 ASMInstCallIMM32(I32 offset) {
    CompilerEmit(0xE8);
    CompilerEmit((offset >> 0) & 0xFF);
    CompilerEmit((offset >> 8) & 0xFF);
    CompilerEmit((offset >> 16) & 0xFF);
    CompilerEmit((offset >> 24) & 0xFF);
}
U0 ASMInstCallReg32(U8 reg) {
    CompilerEmit(0xFF);
    CompilerEmit((0b11 << 6) | (0b010 << 3) | (reg & 0b111));
}