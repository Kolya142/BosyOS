mov eax, cr0
and eax, 0x7fffffff ; who need 01111111111111111111111111111111b
mov cr0, eax
; tables 
mov edi, 0x1000
; mov cr3, edi
xor eax, eax ; fill 0
mov ecx, 1024 ; who need 4096 / 4
rep stosd ; +1 rep to stosd
; mov edi, cr3
; code
mov edi, 0x1000
mov dword [edi], 0x3
add edi, 8
mov dword [edi], 0x10003
add edi, 8
mov dword [edi], 0x20003
add edi, 8
mov dword [edi], 0x30003
mov eax, 0x1000
mov cr3, eax
; noticed already, al fine
mov eax, cr4
or eax, 1 << 5 ; fae <----------------------
mov cr4, eax   ;                           |
; mov ebx, 3 ; who need 0x00000003         |
; mov ecx, 512 ; bios bootloader length    |
; .SetEntry:                               |
;     mov dword [edi], ebx                 |
;     add ebx, 0x1000                      |
;     add edi, 8                           |
;     loop SetupLong.SetEntry              |
;                                          |
; mov eax, cr4                             |
; or eax, 1 << 5 --------------------------|
; mov cr4, eax ; fix hang with valid cr3 table
mov eax, 7
xor ecx, ecx ; set ecx to 0
cpuid
test ecx, 1 << 16
jnz .5_page

mov byte [0xb8005], 4 ; make x red
jmp .pagau_sing
; or ecx, 1 << 12
; mov cr4, eax
.5_page:

mov ecx, 0xC0000080
rdmsr
or eax, 1 << 8 ; 1 there
wrmsr
.pagau_sing:

mov ecx, 0xC0000080
rdmsr
or eax, 1 << 8
wrmsr

mov eax, cr0
or eax, 1 << 31
mov cr0, eax

; .GDT:
;     .Null: equ $ - .GDT
;         dq 0

mov byte [0xb8005], 0xf ; make x white
