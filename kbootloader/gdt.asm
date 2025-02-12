tss_base equ tss_start - ($$-$+$$+$) ; EvilDataTypeHacking
tss_size equ tss_end - tss_base - 1 - ($$-$+$$+$)
tss_t1 equ (tss_base >> 16) & 0xff
tss_t2 equ (tss_base >> 24) & 0xff
gdt_start: ; What is the
   ; 012345678abcdefghi
   dq 0x0000000000000000
   dq 0x00CF9A000000FFFF
   dq 0x00CF92000000FFFF
   dq 0x00AF9A0000000000
   dq 0x00AF920000000000

   dw tss_size
   dw tss_base
   db tss_t1
   db 0x89
   db (tss_size >> 16) & 0xf
   db tss_t2
gdt_end:

tss_start:
   dd 0
   dd 0x300000
   dd 0x10
   dd 0, 0, 0
   dd 0, 0
   dd 0, 0, 0, 0
   dd 0, 0, 0, 0
   dd 0, 0, 0, 0
   dd 0, 0, 0, 0
   dd 0
tss_end:

gdt_descriptor:
   dw gdt_end - gdt_start - 1
   dq gdt_start