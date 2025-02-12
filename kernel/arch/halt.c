void CpuHalt() {
    asm("hlt");
    for(;;);
}