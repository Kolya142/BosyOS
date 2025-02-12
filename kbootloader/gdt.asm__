gdt_start: ; What is the
     ; 012345678abcdefghi
    dq 0x0000000000000000
    dq 0x00CF9A000000FFFF
    dq 0x00CF92000000FFFF
    dq 0x00AF9A0000000000
    dq 0x00AF920000000000
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dq gdt_start
; we need storage
gdtr DW 0
    DD 0