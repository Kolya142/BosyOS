OUTPUT_FORMAT("elf32-i386")
ENTRY(_start)
SECTIONS
{    
    .multiboot : ALIGN(4) { KEEP(*(.multiboot)) }
    . = 0x100000;
    .text : ALIGN(4) { *(.text) }
    .data : ALIGN(4) { *(.data) }
    .bss  : ALIGN(4) {
        *(COMMON)
        *(.bss)
    }

}
