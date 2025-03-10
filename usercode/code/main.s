[bits 32]
section .text
global _start
_start:
    mov eax, 1
    mov esi, msg
    int 0x80
    mov eax, 0x15
    mov esi, 2
    xor ebx, ebx
    int 0x80
section .rodata
msg db "Hello, world!\n"