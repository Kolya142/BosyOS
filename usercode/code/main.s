[bits 32]
section .text
global _start
_start:
    mov eax, 11
    mov esi, _main
    int 0x80

    jmp $
_main:
    mov eax, 4
    mov esi, 1
    mov edi, hello
    mov ebx, 24
    int 0x80
_loop:
    mov ecx, 64
    lea edi, buf
    xor eax, eax
    rep stosb

    mov eax, 4
    mov esi, 1
    mov edi, shell
    mov ebx, 3
    int 0x80

    .read:
        mov eax, 3
        xor esi, esi
        mov edi, buf
        mov ebx, 64
        int 0x80
        test eax, eax
        jz .read

    mov esi, buf
    mov edi, cls
    call strcmp
    test eax, eax
    jz ccls

    mov esi, buf
    mov edi, help
    call strcmp
    test eax, eax
    jz chelp

    mov esi, buf
    mov edi, cat
    call strcmp
    test eax, eax
    jz ccat

    ; unknown command

    mov eax, 4
    mov esi, 1
    mov edi, unk
    mov ebx, 5
    int 0x80

    mov eax, 4
    mov esi, 1
    mov edi, buf
    mov ebx, 9
    int 0x80

    mov eax, 4
    mov esi, 1
    mov edi, mark
    mov ebx, 1
    int 0x80

    jmp _loop

strcmp:
    mov cl, byte [esi]
    mov dl, byte [edi]
    cmp cl, dl
    jne strcmp1
    test cl, cl
    jz strcmp0
    inc esi
    inc edi
    jmp strcmp
strcmp0:
    xor eax, eax
    ret
strcmp1:
    xor eax, eax
    mov al, cl
    sub al, dl
    ret
strncmp:
    mov cl, byte [esi]
    mov dl, byte [edi]
    cmp cl, dl
    jne strcmp1
    test cl, cl
    jz strcmp0
    dec ebx
    test ebx, ebx
    inc esi
    inc edi
    jmp strncmp
ccls:
    mov eax, 4
    mov esi, 1
    mov edi, ctrl
    mov ebx, 1
    int 0x80
    jmp _loop
chelp:
    mov eax, 4
    mov esi, 1
    mov edi, help_msg
    mov ebx, help_msg_len
    int 0x80
    jmp _loop
ccat:
    
    ; Open
    mov eax, 5
    mov esi, buf
    add esi, 4
    int 0x80

    cmp eax, 0
    je _loop

    mov dword [FILE], eax

    ; Read    
    mov eax, 3
    mov esi, dword [FILE]
    mov edi, buf
    mov ebx, 64
    int 0x80

    ; Out
    mov ebx, eax
    mov eax, 4
    mov esi, 1
    mov edi, buf
    int 0x80

    ; Close
    mov eax, 6
    mov esi, dword [FILE]
    int 0x80

    jmp _loop

section .rodata
    hello db "Simple shell application"
    shell db 10, "$ "
    cls db "cls"
    cat db "cat"
    help db "help"
    help_msg db "commands:", 10, "help - show this text", 10, "cls - clear screen"
    help_msg_len equ $ - help_msg
    unk db "unk ", 34
    mark db 34
    ctrl db 0x80
section .bss
    FILE resd 1
    buf resb 64
section .bossec
    dd 0x0000_0000
    dd 0x0000_0001
    dd 0x0C00_0000