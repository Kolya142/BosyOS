[bits 32]
section .text
global RingSwitch

RingSwitch:
    mov ax, (4 * 8) | 3
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov eax, dword [esp + 4]
    push (4 * 8) | 3
    push eax
    pushf
    push (3 * 8) | 3
    push dword [esp + 8]
    iret