[bits 32]
section .text
global _syscall
_syscall:
    push ebx
    push esi
    push edi
    push edx
    
    mov eax, dword [esp + 20]
    mov esi, dword [esp + 24]
    mov edi, dword [esp + 28]
    mov ebx, dword [esp + 32]
    mov edx, dword [esp + 36]
    
    int 0x80

    pop edx
    pop edi
    pop esi
    pop ebx
    ret