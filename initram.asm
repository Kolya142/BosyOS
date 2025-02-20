[bits 32]
[org 0x100000]
mov al, 1
mov esi, msg
int 0x78
mov ecx, 10
l:
    mov al, 6
    mov esi, 10
    sub esi, ecx
    int 0x78
loop l
ret
msg db "press F1-F5", 10, 0