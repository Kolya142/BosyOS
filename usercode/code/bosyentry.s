[bits 32]
section .text
extern main
extern _main
global _start
global start
_start:
    call main
    jmp $

start:
    mov eax, 11
    mov ebx, _main
    int 0x80

    jmp $
section .bossec
dd 0x00000000
dd 0x00000001
dd start