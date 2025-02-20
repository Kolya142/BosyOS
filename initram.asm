[bits 32]
[org 0x100000]
mov al, 1
mov esi, msg
int 0x78
mov ecx, 10
ret
msg db "initram test", 10, 0