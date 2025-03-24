[bits 32]
section .multiboot
align 4
MB_MAGIC     equ 0x1BADB002
MB_FLAGS     equ 0x00000000
MB_CHECKSUM  equ -(MB_MAGIC+MB_FLAGS)

multiboot_header:
    dd MB_MAGIC
    dd MB_FLAGS
    dd MB_CHECKSUM

section .text
global _start
extern KernelMain

_start:
    cli
    mov esp, 0x0600000
    call KernelMain
    hlt