#include <kernel/KPanic.h>
#include <lang/Compiler.h>
#include <lib/IO/TTY.h>

U32 CompilerExpr(String code, List *vars) {
    U32 s = 0;
    Token tok;
    U32 a = 0;
    Char e = ' ';
    U32 sym = 0;
    NEXTTOK
    Bool cont = True;
    CompilerVariable *var;
    for (;a && cont;) {
        SerialPrintF("Tok %s\n", tok.str);
        switch (s) {
            case 0: {
                U8 reg = RegFromName(tok.str);
                if (reg == 0xFF) {
                    if (tok.type == TOK_STR) {
                        ASMInstMovIMM2Reg32(ASM_REG_EBX, CompilerRoDataAdd(tok.str));
                    }
                    else if (!StrCmp(tok.str, "@")) {
                        NEXTTOK
                        U8 reg = RegFromName(tok.str);
                        if (reg == 0xFF) {
                            PrintF("Invalid register in memory access: \"%s\"\n", tok.str);
                            return 0;
                        }
                        ASMInstMovMem2Reg32(ASM_REG_EBX, reg);
                    }
                    else if (!StrCmp(tok.str, "&")) {
                        NEXTTOK
                        if (var = CompilerFindVar(vars, tok.str)) {
                            ASMInstMovReg2Reg32(ASM_REG_EBX, ASM_REG_EBP);
                            ASMInstAddIMM2Reg32(ASM_REG_EBX, -(I32)var->rel);
                        }
                    }
                    else if (!StrCmp(tok.str, "*")) {
                        NEXTTOK
                        CompilerVariable *cvar = CompilerFindVar(vars, tok.str);
                        ASMInstMovDisp2Reg32(ASM_REG_EDX, ASM_REG_EBP, -(I32)cvar->rel, 4);
                        ASMInstMovDisp2Reg32(ASM_REG_EBX, ASM_REG_EDX, 0, 1);
                    }
                    else if (var = CompilerFindVar(vars, tok.str)) {
                        ASMInstMovDisp2Reg32(ASM_REG_EBX, ASM_REG_EBP, -((I32)var->rel), var->type / 8);
                    }
                    else {
                        // PrintF("1");
                        ASMInstMovIMM2Reg32(ASM_REG_EBX, Atoi(tok.str));
                        // PrintF("1");
                    }
                }
                else {
                    ASMInstMovReg2Reg32(ASM_REG_EBX, reg);
                }
                SerialPrintF("s0 %s\n", tok.str);
                s = 1;
            } break;
            case 1: {
                e = tok.str[0];
                SerialPrintF("s1 %c\n", e);
                s = 2;
                if (e != '-' && e != '+' && e != '&' && e != '^' && e != '|' && e != '*' &&
                    e != '/' && e != '%' && e != '<' && e != '>' && e != '!' && e != '=') {
                    cont = False;
                }
            } break;
            case 2: {
                U8 reg = RegFromName(tok.str);
                if (reg == 0xFF) {
                    if (!StrCmp(tok.str, "@")) {
                        NEXTTOK
                        U8 reg = RegFromName(tok.str);
                        if (reg == 0xFF) {
                            PrintF("Invalid register in memory access: \"%s\"\n", tok.str);
                            return 0;
                        }
                        ASMInstMovMem2Reg32(ASM_REG_EDX, reg);
                    }
                    else if (!StrCmp(tok.str, "&")) {
                        NEXTTOK
                        if (var = CompilerFindVar(vars, tok.str)) {
                            ASMInstMovReg2Reg32(ASM_REG_EDX, ASM_REG_EBP);
                            ASMInstAddIMM2Reg32(ASM_REG_EDX, -(I32)var->rel);
                        }
                    }
                    else if (!StrCmp(tok.str, "*")) {
                        NEXTTOK
                        if (var = CompilerFindVar(vars, tok.str)) {
                            ASMInstMovDisp2Reg32(ASM_REG_EDX, ASM_REG_EBP, -(I32)var->rel, var->type / 8);
                        }
                        else if (reg = RegFromName(tok.str)) {
                            ASMInstMovMem2Reg32(ASM_REG_EDX, reg);
                        }
                    }
                    else if (tok.type == TOK_STR) {
                        ASMInstMovIMM2Reg32(ASM_REG_EDX, CompilerRoDataAdd(tok.str));
                    }
                    else if (var = CompilerFindVar(vars, tok.str)) {
                        ASMInstMovDisp2Reg32(ASM_REG_EDX, ASM_REG_EBP, -((I32)var->rel), var->type / 8);
                    }
                    else {
                        ASMInstMovIMM2Reg32(ASM_REG_EDX, Atoi(tok.str));
                    }
                }
                else {
                    ASMInstMovReg2Reg32(ASM_REG_EDX, reg);
                }
                switch (e) {
                    case '-': {
                        ASMInstSubReg2Reg32(ASM_REG_EBX, ASM_REG_EDX);
                    } break;
                    case '+': {
                        ASMInstAddReg2Reg32(ASM_REG_EBX, ASM_REG_EDX);
                    } break;
                    case '*': {
                        ASMInstIMulReg2Reg32(ASM_REG_EBX, ASM_REG_EDX);
                    } break;
                    case '/': {
                        ASMInstMovReg2Reg32(ASM_REG_EAX, ASM_REG_EBX);
                        ASMInstMovReg2Reg32(ASM_REG_ECX, ASM_REG_EDX);
                        ASMInstXorReg2Reg32(ASM_REG_EDX, ASM_REG_EDX);
                        ASMInstIDivReg32(ASM_REG_ECX);
                        ASMInstMovReg2Reg32(ASM_REG_EBX, ASM_REG_EAX);
                    } break;
                    case '%': {
                        ASMInstMovReg2Reg32(ASM_REG_EAX, ASM_REG_EBX);
                        ASMInstMovReg2Reg32(ASM_REG_ECX, ASM_REG_EDX);
                        ASMInstXorReg2Reg32(ASM_REG_EDX, ASM_REG_EDX);
                        ASMInstIDivReg32(ASM_REG_ECX);
                        ASMInstMovReg2Reg32(ASM_REG_EBX, ASM_REG_EDX);
                    } break;
                    case '&': {
                        ASMInstAndReg2Reg32(ASM_REG_EBX, ASM_REG_EDX);
                    } break;
                    case '^': {
                        ASMInstXorReg2Reg32(ASM_REG_EBX, ASM_REG_EDX);
                    } break;
                    case '|': {
                        ASMInstOrReg2Reg32(ASM_REG_EBX, ASM_REG_EDX);
                    } break;
                    case '<': {
                        ASMInstCmpReg2Reg32(ASM_REG_EDX, ASM_REG_EBX);
                        ASMInstMovIMM2Reg32(ASM_REG_EBX, 0);
                        CompilerEmit(0x0F);CompilerEmit(0x92);CompilerEmit(0xC3);
                    } break;
                    case '>': {
                        ASMInstCmpReg2Reg32(ASM_REG_EBX, ASM_REG_EDX);
                        ASMInstMovIMM2Reg32(ASM_REG_EBX, 0);
                        CompilerEmit(0x0F);CompilerEmit(0x92);CompilerEmit(0xC3);
                    } break;
                    case '!': {
                        ASMInstCmpReg2Reg32(ASM_REG_EDX, ASM_REG_EBX);
                        ASMInstMovIMM2Reg32(ASM_REG_EBX, 0);
                        CompilerEmit(0x0F);CompilerEmit(0x95);CompilerEmit(0xC3);
                    } break;
                    case '=': {
                        ASMInstCmpReg2Reg32(ASM_REG_EDX, ASM_REG_EBX);
                        ASMInstMovIMM2Reg32(ASM_REG_EBX, 0);
                        CompilerEmit(0x0F);CompilerEmit(0x94);CompilerEmit(0xC3);
                    } break;
                }
                SerialPrintF("s2 %s\n", tok.str);
                s = 1;
            } break;
            default: {
                PrintF("Stange Activity Detected\nDO PANIC\n");
                KPanic("Strange Compiler Activity", False);
            }
        }
        if (cont) {
            NEXTTOK
        }
    }
    return sym;
}