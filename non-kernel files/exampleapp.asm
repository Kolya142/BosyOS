[bits 32]
[org 0x100000]
mov al, 1
mov esi, msg
int 0x78
jmp $
msg db "$!AHello, world!$!F", 10, 0