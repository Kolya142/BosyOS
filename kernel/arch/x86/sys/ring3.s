[bits 32]
section .text
global RingSwitch
extern TSS
extern SerialPrintF

RingSwitch:
    cli
    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov eax, dword [esp + 4]
    mov ebx, dword [esp + 8]

    mov dword [TSS + 4], ebx

    push dword 0x23
    push ebx
    push dword 0x202
    push dword 0x1B
    push eax
    iret