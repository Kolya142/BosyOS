[bits 16]
[org 0x7c00]

mov ah, 2  ; read
mov al, 2  ; sectors
mov ch, 0  ; cylinder
mov cl, 2  ; first sector
mov dh, 0  ; head
mov bx, 0x800
mov es, bx
mov bx, 0
int 0x13

cli
lgdt [gdt_descriptor]
mov eax, cr0
or eax, 1
mov cr0, eax
jmp 0x08:protected_mode

tss_base equ tss_start - ($$-$+$$+$)
tss_size equ tss_end - tss_base - 1 - ($$-$+$$+$)
tss_t1 equ (tss_base >> 16) & 0xff
tss_t2 equ (tss_base >> 24) & 0xff
gdt_start:
    dq 0x0000000000000000
    dq 0x00CF9A000000FFFF
    dq 0x00CF92000000FFFF
    dq 0x00AF9A000000FFFF
    dq 0x00AF92000000FFFF
    dw tss_size
    dw tss_base
    db tss_t1
    db 0x89
    db (tss_size >> 16) & 0xf
    db tss_t2
gdt_end:

tss_start:
    dd 0
    dd 0x300000
    dd 0x10
    dd 0, 0, 0
    dd 0, 0
    dd 0, 0, 0, 0
    dd 0, 0, 0, 0
    dd 0, 0, 0, 0
    dd 0, 0, 0, 0
    dd 0
tss_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dq gdt_start

[bits 32]
protected_mode:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov word [0xb8000], 0x0F32
    jmp 0x8000    
times 510-($-$$) db 0
dw 0xaa55
