[bits 32]
[org 0]
; sti
hlt
mov eax, 0xDEBFFAFA
mov al, 1
mov esi, msg
int 0x80
jmp $
msg db "$!AHello, world!$!F", 10, 0