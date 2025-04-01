#include <lang/Compiler.h>
#include <lib/IO/TTY.h>

/*
Bugs:
String eats semicolon
*/

#define TOK_CHECK(t) {if (!StrCmp(tok.str, t)) {PrintF("Invalid token. Excepted \""t"\" but got \"%s\"");}}

static List *CompilerOutput;
List CompilerRoData;
List CompilerFunctions;

U0 CompilerEmit(U8 code) {
    ListAppend(CompilerOutput, &code);
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
    // if (name[0] == 'I') {
    //     return Atoi(&name[1]) | 0x80;
    // }
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
U0 CompilerRegsDump() {
    U32 eax, ebx, ecx, edx, esi, edi;
    asmV(
        "mov %%eax, %0\n"
        "mov %%ebx, %1\n"
        "mov %%ecx, %2\n"
        "mov %%edx, %3\n"
        "mov %%esi, %4\n"
        "mov %%edi, %5\n"
        : "=m"(eax), "=m"(ebx), "=m"(ecx), "=m"(edx), "=m"(esi), "=m"(edi)
    );
    PrintF("Regs:\nEAX - %X\nEBX - %X\nECX - %X\nEDX - %X\nESI - %X\nEDI - %X\n", eax, ebx, ecx, edx, esi, edi);
}
CompilerVariable *CompilerFindVar(List *vars, String name) {
    CompilerVariable *varsd = vars->arr;
    for (U32 i = 0; i < vars->count; ++i) {
        CompilerVariable *var = &varsd[i];
        if (!StrCmp(var->name, name)) {
            return var;
        }
    }
    return NULL;
}
U8 CompilerTypeFromName(String name) {
    if (name[0] == 'U') {
        U8 type = Atoi(name + 1);
        if (type != 0 && type != 8 && type != 16 && type != 32) {
            return 0xFF;
        }
        return type;
    }
    if (name[0] == 'I') {
        U8 type = Atoi(name + 1);
        if (type != 0 && type != 8 && type != 16 && type != 32) {
            return 0xFF;
        }
        return type | 0x80;
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

U32 CompilerRoDataAdd(String text) {
    for (U32 i = 0; i < CompilerRoData.count; ++i) {
        if (!StrCmp(text, ((String*)CompilerRoData.arr)[i])) {
            return (U32)((String*)CompilerRoData.arr)[i];
        }
    }
    U32 size = StrLen(text) + 1;
    String str = MAlloc(size);
    MemCpy(str, text, size);
    ListAppend(&CompilerRoData, &str);
    return (U32)str;
}

List Compiler(String code, List parvars) {
    List *prev_output = CompilerOutput;
    List my_output = ListInit(1);
    CompilerOutput = &my_output;
    U32 eip = (U32)CompilerOutput->arr;
    Token tok;
    U32 a = 0;
    U32 s = 0;
    U32 sym = 0;
    U32 enter = 0;
    PrintF("$!BCompiling at %p %p...$!F\n", eip, CompilerOutput);
    NEXTTOK
    CompilerFunction *cfunc;
    CompilerEmit(0x50 + ASM_REG_EBP);
    ASMInstMovReg2Reg32(ASM_REG_EBP, ASM_REG_ESP);
    CompilerEmit(0x81);
    CompilerEmit(0xEC);
    U32 esp_patch = CompilerOutput->count;
    CompilerEmit(0xAA);CompilerEmit(0x55);CompilerEmit(0xEF);CompilerEmit(0xBE);
    for (;a;) {
        PrintF("$!ATok (%s) %d; EIP (%p)$!F\n\n", tok.str, tok.type, eip + CompilerOutput->count);
        // NEXTTOK
        // continue;
        CompilerVariable *cvar = CompilerFindVar(&parvars, tok.str);
        U8 ctype = CompilerTypeFromName(tok.str);
        U8 creg = RegFromName(tok.str);
        if (!StrCmp(tok.str, "Uf")) {
            NEXTTOK
            CompilerFunction *func = get_func(tok.str);
            if (func) {
                PrintF("Disassembling %s:", func->name);
                ASMDis(func->code.arr, func->code.count);
            }
            else {
                PrintF("Failed to Disassembly \"%s\": No such function\n", tok.str);
            }
        }
        else if (!StrCmp(tok.str, "Dump")) {
            ASMInstMovIMM2Reg32(ASM_REG_EDX, (U32)CompilerRegsDump);
            ASMInstCallReg32(ASM_REG_EDX);
        }
        else if (!StrCmp(tok.str, "INT")) {
            NEXTTOK
            CompilerEmit(0xCD);
            CompilerEmit(Atoi(tok.str));
        }
        else if (!StrCmp(tok.str, "IN")) {
            NEXTTOK
            CompilerEmit(ASM_IN);
            CompilerEmit(Atoi(tok.str));
        }
        else if (!StrCmp(tok.str, "OUT")) {
            NEXTTOK
            CompilerEmit(ASM_OUT);
            CompilerEmit(Atoi(tok.str));
        }
        else if (ctype != 0xFF) {
            NEXTTOK
            Char name[32];
            MemSet(name, 0, 32);
            StrCpy(name, tok.str);
            NEXTTOK
            if (!StrCmp(tok.str, "=")) {
                if (CompilerFindVar(&parvars, name)) {
                    PrintF("Variable %s already exists\n", name);
                    return (List) {
                        .count = 0
                    };
                }
                CompilerVariable var;
                MemSet(var.name, 0, 32);
                StrCpy(var.name, name);
                var.rel = parvars.count ? ((CompilerVariable*)parvars.arr)[parvars.count - 1].rel + (ctype / 8) : (ctype / 8);
                PrintF("Let %s; [EBP - %x]\n", name, var.rel);
                var.type = ctype;
                ListAppend(&parvars, &var);
                a = CompilerExpr(code, &parvars);
                sym += a;
                code += a;
                ASMInstMovReg2Disp32(ASM_REG_EBP, ASM_REG_EBX, -((I32)var.rel), ctype / 8);
            }
            else {
                List childvars = ListInit(sizeof(CompilerVariable));
                CompilerFunction func;
                MemSet(func.name, 0, 32);
                StrCpy(func.name, name);
                func.code = Compiler(code - StrLen(tok.str), childvars);
                PrintF("$!dFunction \"%s\"$!F\n", name);
                ListAppend(&CompilerFunctions, &func);
                ListDestroy(&childvars);
                U32 enter1 = 0;
                do {
                    if (!StrCmp(tok.str, "{")) {
                        ++enter1;
                    }
                    if (!StrCmp(tok.str, "}")) {
                        --enter1;
                        if (!enter1) {
                            break;
                        }
                    }
                    NEXTTOK
                } while (a);
            }
        }
        else if (!StrCmp(tok.str, "if")) {
            a = CompilerExpr(code, &parvars);
            sym += a;
            code += a;
            PrintF("if %d\n", a);
            List block = Compiler(code, parvars);
            block.count -= 1;

            ASMInstCmpImm2Reg32(ASM_REG_EBX, 0);

            ASMInstJeIMM32(block.count);
            ListDestroy(&block);
        }
        else if (!StrCmp(tok.str, "while")) {
            a = CompilerExpr(code, &parvars);
            sym += a;
            code += a;
            PrintF("if %d\n", a);
            List block = Compiler(code, parvars);
            block.count -= 1;

            ASMInstCmpImm2Reg32(ASM_REG_EBX, 0);
            ASMInstJeIMM32(block.count + 5);

            for (U32 i = 0; i < block.count; ++i) {
                CompilerEmit(((U8*)block.arr)[i]);
            }

            U32 enter1 = 0;
            do {
                if (!StrCmp(tok.str, "{")) {
                    ++enter1;
                }
                if (!StrCmp(tok.str, "}")) {
                    --enter1;
                    if (!enter1) {
                        break;
                    }
                }
                NEXTTOK
            } while (a);

            ASMInstJmpIMM32(-(I32)block.count - 11);

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
        else if (cvar) {
            a = CompilerExpr(code, &parvars);
            sym += a;
            code += a;
            ASMInstMovReg2Disp32(ASM_REG_EBP, ASM_REG_EBX, -((I32)cvar->rel), cvar->type / 8);
        }
        else if (creg != 0xFF) {
            a = CompilerExpr(code, &parvars);
            sym += a;
            code += a;
            if (creg != ASM_REG_EBX) {
                ASMInstMovReg2Reg32(creg, ASM_REG_EBX);
            }
        }
        else if (!StrCmp(tok.str, ";")) {
            
        }
        else if (cfunc = get_func(tok.str)) {
            ASMInstMovIMM2Reg32(ASM_REG_EDX, (U32)cfunc->code.arr);
            ASMInstCallReg32(ASM_REG_EDX);
        }
        else {
            PrintF("Invalid OpCode \"%s\"\n", tok.str);
            return (List) {
                .count = 0
            };
        }
        NEXTTOK
    }
    
    ASMInstMovReg2Reg32(ASM_REG_ESP, ASM_REG_EBP);
    CompilerEmit(0x58 + ASM_REG_EBP);
    CompilerEmit(0xC3);
    PrintF("\n");
    SerialPrintF("Patching ESP\n");
    U32 *esp_patchv = (CompilerOutput->arr + esp_patch);
    if (*esp_patchv != 0xBEEF55AA) {
        SerialPrintF("ERROR: ESP PATCH INVALID %p\n", *esp_patchv);
    }
    *esp_patchv = (parvars.count ? (4 - (((CompilerVariable*)parvars.arr)[parvars.count - 1].rel % 4)) + 4 : 4);
    CompilerOutput = prev_output;
    return my_output;
}