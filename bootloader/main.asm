; main.asm

; Warning: is doen't work on real hardware or emulator, except QEMU
org 0x7c00
bits 16
jmp text ; Data
BOOTDRIVE db 0
LOADED db 0
msg db "Try to load system kernel...", 0
msg1 db "Failed to load system kernel", 0
msg2 db "Excepted magic 79c5h", 0
msg3 db "Kernel already loaded", 0
msg4 db "Kernel running...", 0
msg5 db "Kernel doen't support big endian", 0
endi dw 0x1234
text: ; Text

mov byte [BOOTDRIVE], dl
mov si, msg
call print

cmp byte [endi], 0x34
jne error3

mov al, byte [LOADED]

test al, al
jnz error2
mov byte [LOADED], 1

mov ah, 0
mov dl, byte [BOOTDRIVE]
int 0x13

mov ah, 2  ; read
mov al, 2  ; 1K
mov ch, 0  ; cylinder
mov cl, 2  ; first sector
mov dh, 0  ; head
mov dl, byte [BOOTDRIVE]
mov bx, 0x800
mov es, bx
mov bx, 0
int 0x13
jc error
cmp word [0x8000], 79c5h
jne error1
mov si, msg4
call print
jmp 0x0000:0x8002

jmp $

error:
    mov si, msg1
    call print
    jmp $

error1:
    mov si, msg2
    call print
    jmp $

error2:
    mov si, msg3
    call print
    jmp $

error3:
    mov si, msg5
    call print
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

times 510-($-$$) db 0
dw 0xaa55