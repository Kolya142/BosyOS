[bits 32]
section .text
extern main
global _start
_start:
    mov eax, 11
    mov esi, main
    int 0x80

    jmp $

section .bossec
dd 0x00000000
dd 0x00000001
dd _start