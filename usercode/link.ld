ENTRY(_start)

SECTIONS {
    . = 0x0C000000;
    _start = .;

    .text :   { *(.text) }
    .data :   { *(.data) }
    .bss  :   { *(.bss) }
    .bossec : {*(.bossec)}
}