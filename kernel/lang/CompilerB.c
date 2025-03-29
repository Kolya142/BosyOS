#include <lang/Compiler.h>
#include <lib/IO/TTY.h>

U32 CompilerExpr(String code) {
    U32 s = 0;
    Token tok;
    U32 a = 0;
    Char e = ' ';
    U32 sym = 0;
    NEXTTOK
    Bool cont = True;
    for (;a && cont;) {
        PrintF("Tok %s\n", tok.str);
        switch (s) {
            case 0: {
                U8 reg = RegFromName(tok.str);
                if (reg == 0xFF) {
                    if (!StrCmp(tok.str, "@")) {
                        NEXTTOK
                        U8 reg = RegFromName(tok.str);
                        if (reg == 0xFF) {
                            PrintF("Invalid register in memory access: \"%s\"\n", tok.str);
                            return 0;
                        }
                        ASMInstMovMem2Reg32(ASM_REG_EBX, reg);
                    }
                    else if (!StrCmp(tok.str, "\"")) {
                        NEXTTOK
                        ASMInstMovIMM2Reg32(ASM_REG_EBX, CompilerRoDataAdd(tok.str));
                    }
                    else {
                        ASMInstMovIMM2Reg32(ASM_REG_EBX, Atoi(tok.str));
                    }
                }
                else {
                    ASMInstMovReg2Reg32(ASM_REG_EBX, reg);
                }
                s = 1;
            } break;
            case 1: {
                e = tok.str[0];
                s = 2;
                if (e != '-' && e != '+' && e != '&' && e != '^' && e != '|' && e != '*') {
                    // sym -= StrLen(tok.str);
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
                    else if (!StrCmp(tok.str, "\"")) {
                        NEXTTOK
                        ASMInstMovIMM2Reg32(ASM_REG_EDX, CompilerRoDataAdd(tok.str));
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
                    case '&': {
                        ASMInstAndReg2Reg32(ASM_REG_EBX, ASM_REG_EDX);
                    } break;
                    case '^': {
                        ASMInstXorReg2Reg32(ASM_REG_EBX, ASM_REG_EDX);
                    } break;
                    case '|': {
                        ASMInstOrReg2Reg32(ASM_REG_EBX, ASM_REG_EDX);
                    } break;
                }
                s = 1;
            } break;
        }
        if (cont) {
            NEXTTOK
        }
    }
    return sym;
}