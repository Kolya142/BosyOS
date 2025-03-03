; boot.asm

[org 0x8000]
bits 16
dw 0x79C5 ; yesiamakernel:)

mov byte [BOOTDRIVE], dl
mov ah, 0x0e
mov al, 'T'
int 0x10

mov ah, 2  ; read
mov al, 127 ; 127 sector
mov ch, 0  ; cylinder
mov cl, 36 ; first sector
mov dh, 0  ; head
mov dl, byte [BOOTDRIVE]
mov bx, 0x9000
mov es, bx
mov bx, 0
int 0x13
jc disk_error

mov ah, 0x0e
mov al, '1'
int 0x10

mov ah, 2  ; read
mov al, 127 ; 127 sector
mov ch, 2  ; cylinder
mov cl, 36+127-63-63 ; first sector
mov dh, 0  ; head
mov dl, byte [BOOTDRIVE]

mov bx, ((0x9000+127*512) >> 4) ; mov this to es
mov es, bx

mov bx, (0x9000+127*512)&0xFFFF

int 0x13
jc disk_error

mov ah, 0x0e
mov al, '2'
int 0x10


; call enable_a20
; call query_a20_support
enable_a20:
    in al, 0x92
    or al, 2
    and al, 0xFE
    out 0x92, al

%include "protected.asm"
[bits 32]
main32:
    mov byte [0xb8000], '3'
    mov byte [0xb8001], 5
    mov byte [0xb8002], '2'
    mov byte [0xb8003], 3
    mov byte [0xb8004], 'x'
    mov byte [0xb8005], 6
    ; %include "long.asm" ; no x64
    jmp kernel_start

; %include "a20-line.asm" ; TODO: make this
%include "gdt.asm"

[bits 32]
kernel_start:
    mov ecx, 0x10000
    mov esi, 0x90000
    mov edi, 0x300000
    rep movsb
    mov ecx, 4000
    mov esi, 0x90000
    mov edi, 0xb8000
    rep movsb
    mov byte [0xb8000+8], 'S'
    mov byte [0xb8002+8], 'U'
    mov byte [0xb8004+8], 'S'
    mov byte [0xb8001+8], 3
    mov byte [0xb8003+8], 5
    mov byte [0xb8005+8], 2
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x600000
    mov ebp, esp
    jmp 0x08:0x300000

disk_error:
    mov si, msg1
    call print
    hlt
    jmp $

print:
    mov al, byte [si]
    test al, al
    jz print.ret
    inc si
    mov ah, 0x0e
    int 0x10
    jmp print
print.ret:
    mov al, 10
    int 0x10
    mov al, 13
    int 0x10
    ret
msg1 db "Disk Read Error", 0
BOOTDRIVE db 0
times 1024-($-$$) db 0