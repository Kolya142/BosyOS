# cd ..
# make kernel
# cd grub
cp ../kernel/kernel.elf iso/boot

dd if=/dev/zero of=bosyos.iso bs=1M count=64

grub2-mkrescue -o bosyos.iso iso
