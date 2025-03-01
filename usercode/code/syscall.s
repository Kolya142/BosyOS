[bits 32]
section .text
global _syscall
_syscall:
    push ebp
    mov ebp, esp
    push ebx
    push esi
    push edi
    push edx
    
    mov eax, dword [ebp + 8]
    mov esi, dword [ebp + 12]
    mov edi, dword [ebp + 16]
    mov ebx, dword [ebp + 20]
    mov edx, dword [ebp + 24]
    
    int 0x80

    pop edx
    pop edi
    pop esi
    pop ebx
    pop ebp
    ret