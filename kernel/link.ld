OUTPUT_FORMAT(binary)
ENTRY(KernelMain)
SECTIONS
{
    . = 0x8400;
    _kernel_start = .;
    .text : { *(.text) }
    . = ALIGN(4);
    .data : { *(.data) }
    .bss : { *(.bss) }
}
