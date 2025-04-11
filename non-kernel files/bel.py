import os
import sys
import ctypes
from typing import List, Tuple


Ast = str | int | List['Ast']

def parser(tokens: List[str]) -> Tuple[Ast, List[str]]:
    ast = []
    while tokens:
        token = tokens[0]
        tokens = tokens[1:]
        if token == '(':
            a, tokens = parser(tokens)
            ast.extend([a])
        elif token == ')':
            break
        else:
            ast.append(token)
    return ast, tokens


def lexer(code: str) -> List[str]:
    tokens = []
    i = 0
    while i < len(code):
        char = code[i]
        if char in "\n\t\r ":
            i += 1
            continue
        if char == '"':
            value = ""
            i += 1
            while i < len(code) and code[i] != '"':
                if code[i] == "\\":
                    i += 1
                    match code[i]:
                        case '"':
                            value += '"'
                        case '\'':
                            value += '\''
                        case '\\':
                            value += '\\'
                        case 'n':
                            value += '\n'
                        case 't':
                            value += '\t'
                        case 'b':
                            value += '\b'
                        case 'e':
                            value += '\x1b'
                        case 'r':
                            value += '\r'
                else:            
                    value += code[i]
                i += 1
            i += 1
            tokens.append(value)
            continue
        if char in "!@#$%^&*()-=_+[]{};:'|\\/?.,><":
            tokens.append(char)
            i += 1
            continue
        if char.isdigit():
            value = ""
            while i < len(code) and code[i].isdigit():
                value += code[i]
                i += 1
            tokens.append(int(value))
            continue
        if char.isalpha() or char == '_':
            value = ""
            while i < len(code) and (code[i].isalpha() or code[i] == '_'):
                value += code[i]
                i += 1
            tokens.append(value)
            continue
    return tokens



libc = ctypes.CDLL(None)
variables = dict()
functions = dict()

def eval(ast: Ast, args: dict = {}) -> object:
    if isinstance(ast, str):
        if ast in variables:
            return variables[ast]
        if ast in args:
            return args[ast]
        if ast in functions:
            return eval(functions[ast][1], args)
    if isinstance(ast, int) or isinstance(ast, str):
        return ast
    if len(ast) == 0:
        return 0
    if isinstance(ast[0], str) and ast[0] in functions:
        if len(ast) != 1:
            for i, arg in enumerate(ast[1:]):
                args[functions[ast[0]][0][i]] = eval(arg)
            return eval(functions[ast[0]][1], args)
        return eval(functions[ast[0]][1], args)
    if ast[0] == "+":
        return eval(ast[1], args) + eval(ast[2], args)
    if ast[0] == "-":
        return eval(ast[1], args) - eval(ast[2], args)
    if ast[0] == "*":
        return eval(ast[1], args) * eval(ast[2], args)
    if ast[0] == "/":
        return eval(ast[1], args) / eval(ast[2], args)
    if ast[0] == "<":
        return 1 if (eval(ast[1], args) < eval(ast[2], args)) else 0
    if ast[0] == ">":
        return 1 if (eval(ast[1], args) > eval(ast[2], args)) else 0
    if ast[0] == "=":
        return 1 if (eval(ast[1], args) == eval(ast[2], args)) else 0
    if ast[0] == "!":
        return 1 if (eval(ast[1], args) != eval(ast[2], args)) else 0
    if ast[0] == "not":
        return 1 if (not eval(ast[1], args)) else 0
    if ast[0] == "endwith":
        return 1 if (str(eval(ast[1], args)).endswith(str(eval(ast[2], args)))) else 0
    if ast[0] == "startwith":
        return 1 if (str(eval(ast[1], args)).startswith(str(eval(ast[2], args)))) else 0
    if ast[0] == "and":
        return 1 if (eval(ast[1], args) and eval(ast[2], args)) else 0
    if ast[0] == "get":
        return eval(ast[1])[eval(ast[2], args)]
    if ast[0] == "len":
        return len(eval(ast[1], args))
    if ast[0] == "async":
        if os.fork() == 0:
            eval(ast[1], args)
            exit(0)
        return 0
    if ast[0] == "eval":
        a = parser(lexer(eval(ast[1], args)))[0]
        # print(a)
        return eval(a)
    if ast[0] == "system":
        return os.system(eval(ast[1], args))
    if ast[0] == "read":
        return open(eval(ast[1], args)).read()
    if ast[0] == "readline":
        try:
            return input()
        except EOFError:
            return ""
    if ast[0] == "listdir":
        return os.listdir()
    if ast[0] == "set":
        r = eval(ast[3])
        eval(ast[1])[eval(ast[2])] = r
        return r
    if ast[0] == "or":
        return 1 if (eval(ast[1], args) or eval(ast[2], args)) else 0
    if ast[0] == "not":
        return 1 if not eval(ast[1], args) else 0
    if ast[0] == "print":
        e = eval(ast[1], args)
        print(e, end="")
        return e
    if ast[0] == "if":
        if eval(ast[1], args):
            return eval(ast[2], args)
        elif len(ast) >= 4:
            return eval(ast[3], args)
    if ast[0] == "while":
        while eval(ast[1], args):
            if ast[2]:
                _ = eval(ast[2], args)
        return _
    if ast[0] == "syscall":
        return libc.syscall(*eval(ast[1]))
    if ast[0] == "let":
        variables[ast[1]] = eval(ast[2], args)
        return 0
    if ast[0] == "list":
        variables[ast[1]] = [0] * eval(ast[2], args)
        return 0
    if ast[0] == "defun":
        functions[ast[1]] = ast[2], ast[3]
        return 0
    res = []
    for a in ast:
        res.append(eval(a, args))
    return res


if __name__ == "__main__":
    t = lexer(open(sys.argv[1]).read())
    print(t)
    a, _ = parser(t)
    print(a)
    eval(a)
