#include <lang/Compiler.h>
#include <lib/IO/TTY.h>

#define TOK_CHECK(t) {if (!StrCmp(tok.str, t)) {PrintF("Invalid token. Excepted \""t"\" but got \"%s\"");}}

static List CompilerOutput;
List CompilerRoData;
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
U8 TypeFromName(String name) {
    if (name[0] == 'U') {
        return Atoi(&name[1]);
    }
    if (name[0] == 'I') {
        return Atoi(&name[1]) | 0x80;
    }
    return 0xFF;
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
    CompilerRoData = ListInit(4);
}

static CompilerFunction *get_func(String name) {
    CompilerFunction *funcs = CompilerFunctions.arr;
    for (U32 i = 0; i < CompilerFunctions.count; ++i) {
        CompilerFunction *func = &funcs[i];
        if (!StrCmp(func->name, name)) {
            return func;
        }
    }
    return NULL;
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
    CompilerFunction *cfunc;
    for (;a;) {
        PrintF("$!ATok (%s); EIP (%p)$!F\n\n", tok.str, eip + CompilerOutput.count);
        if (!StrCmp(tok.str, "Uf")) {
            NEXTTOK
            CompilerFunction *func = get_func(tok.str);
            if (func) {
                PrintF("Disassembling %s:", func->name);
                ASMDis(func->code.arr, func->code.count);
            }
            else {
                PrintF("Failed to Disassembly %s: No such function\n", tok.str);
            }
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
        else if (!StrCmp(tok.str, "fn")) {
            NEXTTOK
            CompilerFunction *func = get_func(tok.str);
            if (func) {
                ListDestroy(&func->code);
                func->code = Compiler(code);
            }
            else {
                CompilerFunction f;
                MemSet(f.name, 0, 32);
                StrCpy(f.name, tok.str);
                f.code = Compiler(code);
                ListAppend(&CompilerFunctions, &f);
                CompilerFunction *funcs = CompilerFunctions.arr;
                func = &funcs[CompilerFunctions.count - 1];
            }

            PrintF("function %s\n", tok.str);

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
            Char e = tok.str[0];
            NEXTTOK
            U8 r2 = RegFromName(tok.str);
            ASMInstCmpReg2Reg32(r2, r1);
            List block = Compiler(code);
            block.count -= 1;
            
            if (e == '>') {
                ASMInstJleIMM32(block.count);
            }
            else if (e == '<') {
                ASMInstJbeIMM32(block.count);
            }
            else if (e == '=') {
                ASMInstJneIMM32(block.count);
            }
            else if (e == '!') {
                ASMInstJeIMM32(block.count);
            }
            PrintF("if %s %c %s\n", RegName(r1), e, RegName(r2));
            ListDestroy(&block);
        }
        else if (!StrCmp(tok.str, "loop")) {
            NEXTTOK
            U8 r1 = RegFromName(tok.str);
            NEXTTOK
            Char e = tok.str[0];
            NEXTTOK
            U8 r2 = RegFromName(tok.str);
            U32 start = CompilerOutput.count;
            ASMInstCmpReg2Reg32(r2, r1);
            List block = Compiler(code);
            block.count -= 1;
            
            if (e == '>') {
                ASMInstJleIMM32(block.count + 5);
            }
            else if (e == '<') {
                ASMInstJbeIMM32(block.count + 5);
            }
            else if (e == '=') {
                ASMInstJneIMM32(block.count + 5);
            }
            else if (e == '!') {
                ASMInstJeIMM32(block.count + 5);
            }
            PrintF("loop %s %c %s\n", RegName(r1), e, RegName(r2));
            for (U32 i = 0; i < block.count; ++i) {
                CompilerEmit(((U8*)block.arr)[i]);
            }
            ASMInstJmpIMM32(-(CompilerOutput.count - start + 5));
            ListDestroy(&block);
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
        else if (!StrCmp(tok.str, ";")) {
            NEXTTOK
        }
        else if (!StrCmp(tok.str, "MOV")) {
            NEXTTOK
            U8 r1 = RegFromName(tok.str);
            NEXTTOK
            U8 r2 = RegFromName(tok.str);
            if (r2 == 0xFF) {
                ASMInstMovIMM2Reg32(r1, Atoi(tok.str));
            }
            else {
                ASMInstMovReg2Reg32(r1, r2);
            }
        }
        else if (!StrCmp(tok.str, "@")) {
            NEXTTOK
            U8 reg = RegFromName(tok.str);
            a = CompilerExpr(code);
            sym += a;
            code += a;
            ASMInstMovReg2Mem32(reg, ASM_REG_EBX);
        }
        else if (!StrCmp(tok.str, "INT"))
        {
            NEXTTOK
            CompilerEmit(0xCD);
            CompilerEmit(Atoi(tok.str));
        }
        
        else if (cfunc = get_func(tok.str)) {
            ASMInstMovIMM2Reg32(ASM_REG_EDX, (U32)cfunc->code.arr);
            ASMInstCallReg32(ASM_REG_EDX);
        }
        else {
            PrintF("Invalid OpCode %s\n", tok.str);
            return (List) {
                .count = 0
            };
        }
        NEXTTOK
    }
    
    CompilerEmit(0xC3);
    PrintF("\n");
    List curr_output = CompilerOutput;
    CompilerOutput = prev_output;
    return curr_output;
}