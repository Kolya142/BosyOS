#include <lang/Compiler.h>
#include <lib/IO/TTY.h>

#define TOK_CHECK(t) {if (!StrCmp(tok.str, t)) {PrintF("Invalid token. Excepted \""t"\" but got \"%s\"");}}

static List CompilerOutput;
List CompilerFunctions;

U0 CompilerEmit(U8 code) {
    ListAppend(&CompilerOutput, &code);
}
String RegName(U8 reg) {
    switch (reg) {
        case ASM_REG_EAX: {
            return "EAX";
        } break;
        case ASM_REG_EBX: {
            return "EBX";
        } break;
        case ASM_REG_ECX: {
            return "ECX";
        } break;
        case ASM_REG_EDX: {
            return "EDX";
        } break;
        case ASM_REG_ESI: {
            return "ESI";
        } break;
        case ASM_REG_EDI: {
            return "EDI";
        } break;
        case ASM_REG_ESP: {
            return "ESP";
        } break;
        case ASM_REG_EBP: {
            return "EBP";
        } break;
    }
    return "INV";
}
U8 RegFromName(String name) {
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
    PrintF("$!BCompiling at %p...$!F\n", eip);
    NEXTTOK
    for (;a;) {
        PrintF("$!ATok (%s); EIP (%p)$!F\n\n", tok.str, eip + CompilerOutput.count);
        if (!StrCmp(tok.str, "fn")) {
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
                PrintF("$!CFunction %s at %p$!F\n", tok.str, funcs[CompilerFunctions.count - 1].code.arr);
            }
            U32 enter1 = 0;
            do {
                NEXTTOK
                if (!StrCmp(tok.str, "{")) {
                    ++enter1;
                }
                else if (!StrCmp(tok.str, "}")) {
                    --enter1;
                }
            } while (a && enter1);
        }
        else if (!StrCmp(tok.str, "if")) {
            NEXTTOK
            U8 r1 = RegFromName(tok.str);
            NEXTTOK
            Char c = tok.str[0];
            NEXTTOK
            U8 r2 = RegFromName(tok.str);
            ASMInstCmpReg2Reg32(r2, r1);
            List new = Compiler(code);
            new.count -= 1;
            if (c == '>') {
                ASMInstJleIMM32(new.count);
            }
            else if (c == '<') {
                ASMInstJbeIMM32(new.count);
            }
            else if (c == '=') {
                ASMInstJneIMM32(new.count);
            }
            else if (c == '!') {
                ASMInstJeIMM32(new.count);
            }
            PrintF("if %s %c %s\n", RegName(r1), c, RegName(r2));
            ListDestroy(&new);
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
        else if (!StrCmp(tok.str, "Uf")) {
            NEXTTOK
            CompilerFunction *funcs = CompilerFunctions.arr;
            for (U32 i = 0; i < CompilerFunctions.count; ++i) {
                if (!StrCmp(funcs[i].name, tok.str)) {
                    PrintF("Uf for %s at %p\n", tok.str, funcs[i].code.arr);
                    ASMDis(funcs[i].code.arr, funcs[i].code.count);
                    break;
                }
            }
        }
        else if (!StrCmp(tok.str, "Comment")) {
            NEXTTOK
            NEXTTOK
        }
        else if (!StrCmp(tok.str, "sys")) {
            CompilerEmit(0xCD);
            CompilerEmit(0x80);
        }
        else if (RegFromName(tok.str) != 0xFF) {
            U8 reg = RegFromName(tok.str);
            a = CompilerExpr(code);
            sym += a;
            code += a;
            if (reg != ASM_REG_EBX) {
                ASMInstMovReg2Reg32(reg, ASM_REG_EBX);
            }
        }
        else if (tok.str[0] == '@') {
            U8 reg = RegFromName(tok.str+1);
            a = CompilerExpr(code);
            sym += a;
            code += a;
            ASMInstMovReg2Mem32(reg, ASM_REG_EBX);
        }
        else {
            Bool is_func_call = False;
            CompilerFunction *funcs = CompilerFunctions.arr;
            for (U32 i = 0; i < CompilerFunctions.count; ++i) {
                if (!StrCmp(funcs[i].name, tok.str)) {
                    ASMInstMovIMM2Reg32(ASM_REG_EDX, (U32)funcs[i].code.arr);
                    ASMInstCallReg32(ASM_REG_EDX);
                    is_func_call = True;
                    break;
                }
            }
            if (!is_func_call && StrCmp(tok.str, ";")) {
                PrintF("Invalid Command \"%s\"\n", tok.str);
                return (List) {
                    .count = 0
                };
            }
        }
        PrintF("Tokk %s\n", tok.str);
        NEXTTOK
    }
    
    CompilerEmit(0xC3);
    PrintF("\n");
    List curr_output = CompilerOutput;
    CompilerOutput = prev_output;
    return curr_output;
}