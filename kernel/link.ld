OUTPUT_FORMAT(binary)
ENTRY(_start)
SECTIONS
{
    . = 0x100000;
    _start = .;
    
    .text : ALIGN(4) { *(.text) }
    .data : ALIGN(4) { *(.data) }
    .bss  : ALIGN(4) { *(.bss) }

}
