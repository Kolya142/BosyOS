mkdir -p build
cd code
gcc -c main.c -o ../build/main.o -m32 -static -fno-toplevel-reorder -mgeneral-regs-only -ffreestanding -nostdlib
gcc -c syscall.c -o ../build/syscall.o -m32 -static -fno-toplevel-reorder -mgeneral-regs-only -ffreestanding -nostdlib
cd ..
ld -T link.ld -o usercode.o build/* -melf_i386
objcopy -O binary usercode.o usercode.bin
objcopy --dump-section .bossec=usercode.bin.bossec usercode.o