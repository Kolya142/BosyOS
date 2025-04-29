cp ../kernel/kernel.elf iso/boot

dd if=/dev/zero of=bosyos2.iso bs=1M count=64

grub-mkrescue -o bosyos2.iso iso