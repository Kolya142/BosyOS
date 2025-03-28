#include <lang/Compiler.h>
#include <lib/IO/TTY.h>

static List CompilerOutput;
List CompilerFunctions;

U0 CompilerEmit(U8 code) {
    ListAppend(&CompilerOutput, &code);
}
static U8 reg_from_name(String name) {
    if (!StrCmp(name, "eax")) {
        return ASM_REG_EAX;
    }
    if (!StrCmp(name, "ebx")) {
        return ASM_REG_EBX;
    }
    if (!StrCmp(name, "ecx")) {
        return ASM_REG_ECX;
    }
    if (!StrCmp(name, "edx")) {
        return ASM_REG_EDX;
    }
    if (!StrCmp(name, "esp")) {
        return ASM_REG_ESP;
    }
    if (!StrCmp(name, "ebp")) {
        return ASM_REG_EBP;
    }
    if (!StrCmp(name, "edi")) {
        return ASM_REG_EDI;
    }
    if (!StrCmp(name, "esi")) {
        return ASM_REG_ESI;
    }
    return 0xFF;
}

U0 CompilerInit() {
    CompilerFunctions = ListInit(sizeof(CompilerFunction));
}

List Compiler(String code) {
    List prev_output = CompilerOutput;
    CompilerOutput = ListInit(1);
    U32 eip = (U32)CompilerOutput.arr;
    Token tok;
    U32 a = 0;
    U32 s = 0;
    U32 sym = 0;
    U32 enter = 0;
    PrintF("Compiling at %p...\n", eip);
    NEXTTOK
    for (;a;) {
        PrintF("Tok (%s); EIP (%p)\n\n", tok.str, eip + CompilerOutput.count);
        if (!StrCmp(tok.str, "def")) {
            NEXTTOK
            CompilerFunction *funcs = CompilerFunctions.arr;
            Bool founded = False;
            for (U32 i = 0; i < CompilerFunctions.count; ++i) {
                if (!StrCmp(funcs[i].name, tok.str)) {
                    ListDestroy(&funcs[i].code);
                    funcs[i].code = Compiler(code);
                    founded = True;
                    break;
                }
            }
            if (!founded) {
                CompilerFunction func;
                func.code = Compiler(code);
                MemSet(func.name, 0, 32);
                StrCpy(func.name, tok.str);
                ListAppend(&CompilerFunctions, &func);
                PrintF("Function %s at %p\n", tok.str, funcs[CompilerFunctions.count - 1].code.arr);
            }
            U32 enter1 = 0;
            do {
                NEXTTOK
                if (!StrCmp(tok.str, "{")) {
                    ++enter1;
                }
                if (!StrCmp(tok.str, "}")) {
                    --enter1;
                }
            } while (a && enter1);
        }
        else if (!StrCmp(tok.str, "{")) {
            ++enter;
        }
        else if (!StrCmp(tok.str, "}")) {
            --enter;
            if (!enter) {
                break;
            }
        }
        else if (!StrCmp(tok.str, "run")) {
            NEXTTOK
            CompilerFunction *funcs = CompilerFunctions.arr;
            Bool founded = False;
            for (U32 i = 0; i < CompilerFunctions.count; ++i) {
                if (!StrCmp(funcs[i].name, tok.str)) {
                    ASMInstMovIMM2Reg32(ASM_REG_EDI, (U32)funcs[i].code.arr);
                    ASMInstCallReg32(ASM_REG_EDI);
                    founded = True;
                    break;
                }
                PrintF("function \"%s\"\n", funcs[i].name);
            }
            if (!founded) {
                PrintF("Failed to found function \"%s\"\n", tok.str);
                ListDestroy(&CompilerOutput);
                return (List) {
                    .count = 0
                };
            }
        }
        else if (!StrCmp(tok.str, "let")) {
            NEXTTOK
            U8 r1 = reg_from_name(tok.str);
            if (r1 == 0xFF) {
                PrintF("Invalid Register Name %s\n", tok.str);
                ListDestroy(&CompilerOutput);
                return (List) {
                    .count = 0
                };
            }
            NEXTTOK
            U8 r2 = reg_from_name(tok.str);
            if (r2 == 0xFF) {
                ASMInstMovIMM2Reg32(r1, Atoi(tok.str));
            }
            else {
                ASMInstMovReg2Reg32(r1, r2);
            }
        }
        else if (!StrCmp(tok.str, "uf")) {
            NEXTTOK
            CompilerFunction *funcs = CompilerFunctions.arr;
            for (U32 i = 0; i < CompilerFunctions.count; ++i) {
                if (!StrCmp(funcs[i].name, tok.str)) {
                    PrintF("UF %s at %p:\n    ", tok.str, funcs[i].code.arr);
                    for (U32 i = 0; i < funcs->code.count; ++i) {
                        PrintF("%1X ", ((U8*)funcs->code.arr)[i]);
                    }
                    PrintF("\n");
                    break;
                }
            }
        }
        else if (!StrCmp(tok.str, "return")) {
            NEXTTOK
            U8 reg = reg_from_name(tok.str);
            if (reg == 0xFF) {
                ASMInstMovIMM2Reg32(ASM_REG_EAX, Atoi(tok.str));
            }
            else {
                ASMInstMovReg2Reg32(ASM_REG_EAX, reg);
            }
            CompilerEmit(0xC3);
        }
        else {
            PrintF("Compilation Error: Invalid OpCode at %d symbol \"%s\"\n", sym, tok.str);
            ListDestroy(&CompilerOutput);
            return (List) {
                .count = 0
            };
        }
        NEXTTOK
    }
    
    CompilerEmit(0xC3);
    for (U32 i = 0; i < CompilerOutput.count; ++i) {
        PrintF("%1X ", ((U8*)CompilerOutput.arr)[i]);
    }
    PrintF("\n");
    List curr_output = CompilerOutput;
    CompilerOutput = prev_output;
    return curr_output;
}