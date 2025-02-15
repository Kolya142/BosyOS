[bits 32]
[org 0x5000]
mov ax, 1
mov esi, msg
int 0x78
ret
msg db "$!AHello, world!$!F", 10, 0