OUTPUT_FORMAT("elf32-i386")
ENTRY(_start)
SECTIONS
{    
    . = 1M;
    .multiboot2 : ALIGN(4) { KEEP(*(.multiboot2)) }
    .text : ALIGN(4) { *(.text) }
    .data : ALIGN(4) { *(.data) }
    .bss  : ALIGN(4) {
        *(COMMON)
        *(.bss)
    }
}