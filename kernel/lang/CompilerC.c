#include <lang/Compiler.h>
#include <lib/IO/TTY.h>

U0 ASMInstMake32(Bool stmode, U8 uses, U8 inst, U8 modrm, U8 sib, U32 disp, U32 imm) {
    if (stmode) {
        CompilerEmit(0x66);
    }
    CompilerEmit(inst);
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

U0 ASMDis(U8* code, U32 count) {
    for (U32 i = 0; i < count; ++i) {
        U8 byte = code[i];
        SerialPrintF("%1X ", byte);
    }
    SerialPrintF("\n");
    for (U32 i = 0; i < count;) {
        U8 byte = code[i];
        SerialPrintF("$!7%p$!F: $!E0x%1X$!F ", code + i, byte);

        switch (byte) {
            case 0x01: { // ADD r/m32, r32
                U8 modrm = code[i + 1];
                U8 dst = modrm & 7;
                U8 src = (modrm >> 3) & 7;
                SerialPrintF("ADD   $!A%s$!F, $!B%s$!F\n", RegName(dst), RegName(src));
                i += 2;
            } break;

            case 0x81: { // SUB r/m32, imm
                U8 modrm = code[i + 1];
                U8 dst = modrm & 7;
                U8 src = (modrm >> 3) & 7;
                U32 imm = *(U32*)&code[i + 2];
                SerialPrintF("SUB   $!A%s$!F, $!B%p$!F\n", RegName(dst), imm);
                i += 6;
            } break;

            case 0x29: { // SUB r/m32, r32
                U8 modrm = code[i + 1];
                U8 dst = modrm & 7;
                U8 src = (modrm >> 3) & 7;
                SerialPrintF("SUB   $!A%s$!F, $!B%s$!F\n", RegName(dst), RegName(src));
                i += 2;
            } break;

            case 0x50 ... 0x57: { // push r32
                U8 reg = byte - 0x50;
                SerialPrintF("PUSH  $!A%s$!F\n", RegName(reg));
                i += 1;
            } break;

            case 0x58 ... 0x5F: { // pop r32
                U8 reg = byte - 0x58;
                SerialPrintF("POP   $!A%s$!F\n", RegName(reg));
                i += 1;
            } break;

            case 0x8B: { // mov r/m32
                U8 modrm = code[i + 1];
                U8 dst = (modrm >> 3) & 7;
                U8 src = modrm & 7;
                if (src != 0b100 || code[i + 2] == 0x24) {
                    SerialPrintF("MOV   $!A%s$!F, $!B[%s]$!F\n", RegName(dst), RegName(src));
                    if (code[i + 2] == 0x24) {
                        ++i;
                    }
                }
                else {
                    U8 sib = code[i];
                    U8 scale = (sib >> 6) & 3;
                    U8 index = (sib >> 3) & 7;
                    U8 base  = sib & 7;
                    SerialPrintF("MOV   $!A%s$!F, $!B[%s+%s*%d]\n", RegName(dst), RegName(base), RegName(index), 1 << scale);
                    ++i;
                }
                i += 2;
            } break;

            case 0xB8 ... 0xBF: { // mov r32, imm32
                U8 reg = byte - 0xB8;
                U32 imm = *(U32*)&code[i + 1];
                SerialPrintF("MOV   $!A%s$!F, $!B%p$!F\n", RegName(reg), imm);
                i += 5;
            } break;

            case 0x89: { // mov r/m32, r32
                U8 modrm = code[i + 1];
                U8 dst = modrm & 7;
                U8 src = (modrm >> 3) & 7;
                SerialPrintF("MOV   $!A%s$!F, $!B%s$!F\n", RegName(dst), RegName(src));
                i += 2;
            } break;

            case 0xE8: { // call rel32
                I32 rel = *(I32*)&code[i + 1];
                U8* target = code + i + 5 + rel;
                SerialPrintF("CALL  $!A%p$!F\n", target);
                i += 5;
            } break;

            case 0xFF: { // call r32
                U8 modrm = code[i + 1];
                U8 reg = (modrm >> 3) & 7;
                U8 rm = modrm & 7;
                if (reg == 2) {
                    SerialPrintF("CALL  $!A%s$!F\n", RegName(rm));
                } else {
                    SerialPrintF("FF    ModRM=0x%1X (unhandled)\n", modrm);
                }
                i += 2;
            } break;

            case 0xC3: {
                SerialPrintF("RET\n");
                i += 1;
            } break;

            default: {
                SerialPrintF("DB    $!A0x%1X$!F\n", byte);
                i += 1;
            } break;
        }
    }
}

U0 ASMInstMovReg2Reg32(U8 dst, U8 src) {
    ASMInstMake32(0, 1, ASM_MOV_R2R, 0b11000000 | (src << 3) | dst, 0, 0, 0);
}
U0 ASMInstMovIMM2Reg32(U8 dst, U32 imm) {
    ASMInstMake32(0, 8, ASM_MOV_IMM + dst, 0, 0, 0, imm);
}
U0 ASMInstMovReg2Mem32(U8 dst, U8 src) {
    ASMInstMake32(0, 1, ASM_MOV_R2R, 0b00000000 | (src << 3) | dst, 0, 0, 0);
}
U0 ASMInstMovReg2Disp32(U8 dst, U8 src, I32 rel, U8 bytes) {
    U8 opcode;
    Bool pref = 0;

    switch (bytes) {
        case 1:  opcode = 0x88; break; // mov [mem], r8
        case 2: opcode = ASM_MOV_R2R; pref = 1; break; // mov [mem], r16
        case 4: opcode = ASM_MOV_R2R; break; // mov [mem], r32
        default: return;
    }

    if (dst == ASM_REG_ESP) {
        ASMInstMake32(pref, 1|2|4, opcode, 0b10000000 | (src << 3) | dst, 0x24, rel, 0);
    }
    else {
        ASMInstMake32(pref, 1|4, opcode, 0b10000000 | (src << 3) | dst, 0, rel, 0);
    }
}
U0 ASMInstMovDisp2Reg32(U8 dst, U8 src, I32 rel, U8 bytes) {
    U8 opcode;
    Bool pref = 0;

    switch (bytes) {
        case 1:  opcode = 0x8A; break; // mov [mem], r8
        case 2: opcode = ASM_MOV_RR; pref = 1; break; // mov [mem], r16
        case 4: opcode = ASM_MOV_RR; break; // mov [mem], r32
        default: return;
    }
    if (bytes != 4) {
        ASMInstXorReg2Reg32(dst, dst);
    }
    if (src == ASM_REG_ESP) {
        ASMInstMake32(pref, 1|2|4, opcode, 0b10000000 | (dst << 3) | src, 0x24, rel, 0);
    }
    else {
        ASMInstMake32(pref, 1|4, opcode, 0b10000000 | (dst << 3) | src, 0, rel, 0);
    }
}
U0 ASMInstMovMem2Reg32(U8 dst, U8 src) {
    if (src == 0b100) {
        CompilerEmit(ASM_MOV_RR);
        CompilerEmit(0b00000000 | (dst << 3) | src);
        CompilerEmit(0x24);
    }
    else {
        ASMInstMake32(0, 1, ASM_MOV_RR, 0b00000000 | (dst << 3) | src, 0, 0, 0);
    }
}

U0 ASMInstAddReg2Reg32(U8 dst, U8 src) {
    ASMInstMake32(0, 1, ASM_ADD, 0b11000000 | (src << 3) | dst, 0, 0, 0);
}
U0 ASMInstAddIMM2Reg32(U8 reg, U32 imm) {
    ASMInstMake32(0, 1|8, ASM_ADD_IMM, 0b11000000 | reg, 0, 0, imm);
}
U0 ASMInstIMulReg2Reg32(U8 dst, U8 src) {
    CompilerEmit(0x0F);
    ASMInstMake32(0, 1, ASM_IMUL, 0b11000000 | (dst << 3) | src, 0, 0, 0);
}
U0 ASMInstIDivReg32(U8 reg) {
    ASMInstMake32(0, 1, ASM_IDIV, 0b11111000 | reg, 0, 0, 0);
}
U0 ASMInstSubReg2Reg32(U8 dst, U8 src) {
    ASMInstMake32(0, 1, ASM_SUB, 0b11000000 | (src << 3) | dst, 0, 0, 0);
}
U0 ASMInstXorReg2Reg32(U8 dst, U8 src) {
    ASMInstMake32(0, 1, ASM_XOR, 0b11000000 | (src << 3) | dst, 0, 0, 0);
}
U0 ASMInstOrReg2Reg32(U8 dst, U8 src) {
    ASMInstMake32(0, 1, ASM_OR, 0b11000000 | (src << 3) | dst, 0, 0, 0);
}
U0 ASMInstAndReg2Reg32(U8 dst, U8 src) {
    ASMInstMake32(0, 1, ASM_AND, 0b11000000 | (src << 3) | dst, 0, 0, 0);
}
U0 ASMInstNotReg2Reg32(U8 reg) {
    ASMInstMake32(0, 1, ASM_NOT, 0b11010000 | reg, 0, 0, 0);
}
U0 ASMInstJccIMM32(U8 opcode, I32 offset) {
    CompilerEmit(0x0F);
    CompilerEmit(opcode);
    CompilerEmit((offset >> 0) & 0xFF);
    CompilerEmit((offset >> 8) & 0xFF);
    CompilerEmit((offset >> 16) & 0xFF);
    CompilerEmit((offset >> 24) & 0xFF);
}
U0 ASMInstJmpIMM32(I32 offset) {
    CompilerEmit(ASM_JMP);
    CompilerEmit((offset >> 0) & 0xFF);
    CompilerEmit((offset >> 8) & 0xFF);
    CompilerEmit((offset >> 16) & 0xFF);
    CompilerEmit((offset >> 24) & 0xFF);
}
U0 ASMInstCmpReg2Reg32(U8 r1, U8 r2) {
    ASMInstMake32(0, 1, 0x3B, 0b11000000 | (r2 << 3) | r1, 0, 0, 0);
}
U0 ASMInstCmpImm2Reg32(U8 reg, U32 imm) {
    ASMInstMake32(0, 1 | 8, 0x81, 0b11111000 | reg, 0, 0, imm);
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
    CompilerEmit(0b11010000 | reg);
}