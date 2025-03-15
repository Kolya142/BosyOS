[bits 32]
section .text
global RingSwitch

RingSwitch:
    cli
    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov eax, dword [esp + 4]
    mov ebx, dword [esp + 8]
    push 0x23
    push ebx
    pushf
    push 0x1B
    push eax
    iret