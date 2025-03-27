[bits 32]
section .multiboot2
align 8
MB2_MAGIC     equ 0xE85250D6
MB2_ARCH      equ 0
MB2_LENGTH   equ multiboot2_end - multiboot2_start
MB2_CHECKSUM equ -(MB2_MAGIC + MB2_ARCH + MB2_LENGTH)

multiboot2_start:
    dd MB2_MAGIC
    dd MB2_ARCH
    dd MB2_LENGTH
    dd MB2_CHECKSUM

    dw 5
    dw 0
    dd 20
    dd 640
    dd 480
    dd 32

align 8
multiboot2_end:

section .text
global _start
extern KernelMain

_start:
    cli
    mov esp, 0x0600000
    push ebx
    call KernelMain
    hlt