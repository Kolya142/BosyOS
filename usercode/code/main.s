[bits 32]
section .text
global _start
_start:
    mov eax, 4
    mov esi, 1
    mov edi, msg
    mov ebx, 14
    int 0x80
    mov eax, 1
    xor esi, esi
    int 0x80
section .rodata
msg db "Hello, world!\n"
section .bossec
dd 0x0000_0000
dd 0x0000_0001
dd _start