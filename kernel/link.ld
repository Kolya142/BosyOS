OUTPUT_FORMAT(binary)
ENTRY(KernelMain)
SECTIONS
{
    . = 0x300000;
    KernelMain = .;
    
    .text : ALIGN(4) { *(.text) }
    .data : ALIGN(4) { *(.data) }
    .bss  : ALIGN(4) { *(.bss) }

}
