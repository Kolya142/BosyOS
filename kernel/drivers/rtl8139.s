[bits 32]
section .text
global RTL8139Start
extern TTYUPrint
extern PrintF
global RTL8139IO
global RTL8139IRQ

; TODO: fix qemu hardcode

RTL8139Start:
    ; Search RTL8139
    mov ecx, 0x00000000
    .search_loop:
        mov dx, 0x0CF8
        mov eax, 0x80000000
        or eax, ecx
        out dx, eax
        mov dx, 0x0CFC
        in eax, dx
        cmp eax, 0x813910EC ; Realtek RTL8139 (Vendor: 10EC, Device: 8139)
        je .success1
        add ecx, 0x800
        cmp ecx, 0x80002000
        jne .search_loop
    .not_found:
        push msg1
        call TTYUPrint
        add esp, 4
        ret
    .success1:
        push ecx
        push msg2
        call PrintF
        add esp, 4*2
    
    ; Enable
    .enable: 
        ; POut32(0x0CF8, 0x80000004 | driveid)
        ; POut32(0x0CF8, PIn32(0x0CFC))
        mov dx, 0x0CF8
        mov eax, 0x80000004
        or eax, ecx
        out dx, eax
        mov dx, 0xCFC
        in eax, dx
        or eax, 0x07
        out dx, eax
    
    .enable_debug:
        push eax
        push msg4
        call PrintF
        add esp, 8

    ; Get IO addr
    .get_id:
        ; POut(0x0CF8, driveid | 0x80000010)
        ; RTL8139IO = PIn(0x0CFC) & 0xFFFFFFFC
        ; mov dx, 0x0CF8

        ; mov eax, 080000010
        ; or eax, ecx

        ; or eax, ecx
        ; out dx, eax
        ; mov dx, 0x0CFC
        ; in eax, dx
        ; and eax, 0xFFFFFFFC

        ; test eax, eax
        ; jz .set_id

        ; mov [RTL8139IO], eax
        ; jmp .enable2
    .set_id:
        mov eax, 0xC000
        mov [RTL8139IO], eax

    ; Enable(part 2)
    .enable2:
        ; POut(RTL8139IO + 0x52, 0)
        mov edx, dword [RTL8139IO]
        add edx, 0x52
        mov al, 0
        out dx, al

    .get_irq:
        ; POut32(0x0CF8, 0x8000003C|deviceid)
        ; RTL8139IRQ = PIn(0xCFC)
        ; mov dx, 0x0CF8
        ; mov eax, ecx
        ; and eax, 0xFFFFF800
        ; or eax, 0x8000003C
        ; out dx, eax
        ; mov dx, 0x0CFC
        ; xor eax, eax
        ; in eax, dx
        
        ; test eax, eax
        ; jz .set_irq

        ; mov [RTL8139IRQ], eax
        ; jmp .debug
    .set_irq:
        mov eax, 11
        mov [RTL8139IRQ], eax
    
    .debug:
        push dword [RTL8139IRQ]
        push dword [RTL8139IO]
        push msg3
        call PrintF
        add esp, 4*3
        
    .exit:
        push msg
        call TTYUPrint
        add esp, 4
        ret

; Global Data
section .bss
RTL8139IO resd 1
RTL8139IRQ resd 1

; Local Data
section .rodata
msg db "RTL8139 setuped", 10, 0
msg1 db "RTL8139 not founded", 10, 0
msg2 db "RTL8139 founded at id %x", 10, 0
msg3 db "IO: %x, IRQ: %x", 10, 0
msg4 db "Enable data: %x", 10, 0