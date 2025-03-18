[bits 32]
section .text
global _syscall
_syscall:
    push ebx
    push esi
    push edi
    push ebp
    
    mov eax, [esp + 20]  ; a
    mov ebx, [esp + 24]  ; b
    mov ecx, [esp + 28]  ; c
    mov edx, [esp + 32]  ; d
    mov esi, [esp + 36]  ; e
    mov edi, [esp + 40]  ; f
    mov ebp, [esp + 44]  ; g

    int 0x80

    pop ebp
    pop edi
    pop esi
    pop ebx
    ret
