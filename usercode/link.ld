ENTRY(main)

SECTIONS {
    . = 0x2000000;

    .text :   { *(.text) }
    .data :   { *(.data) }
    .bss  :   { *(.bss) }
    .bossec : {*(.bossec)}
}