[bits 32]
section .text
global _start
_start:
    ; mov eax, 4
    ; mov ebx, 1
    ; mov ecx, msg
    ; mov edx, 5+2+3+1
    ; int 0x80
    jmp $
    mov eax, 1
    xor ebx, ebx
    int 0x80
    ; jmp $

section .rodata
msg db "Hello, ELF", 10