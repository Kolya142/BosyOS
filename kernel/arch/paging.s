[bits 32]
section .text
global PagingEnable
global Paginginvlpg

PagingEnable:
    mov eax, 0x00D00000
    mov cr3, eax

    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    mov word [0xb8000], 0x0F32
    ret

Paginginvlpg:
    mov eax, [esp + 4]
    invlpg [eax]
    ret
