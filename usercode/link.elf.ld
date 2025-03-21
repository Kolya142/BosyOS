OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

SECTIONS {
    # . = 0x0C000000;
    _start = .;

    .text ALIGN(4K) : {
        *(.text)
    }

    .rodata ALIGN(4K) : {
        *(.rodata)
    }

    .data ALIGN(4K) : {
        *(.data)
    }

    .bss ALIGN(4K) : {
        *(COMMON)
        *(.bss)
    }

    _end = .;
}