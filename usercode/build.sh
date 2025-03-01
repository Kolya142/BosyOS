mkdir -p build
rm build/* -r
set -e
cd code
gcc -c main.c -o ../build/main.o -m32 -static -fno-toplevel-reorder -mgeneral-regs-only -ffreestanding -nostdlib -mhard-float -mfpmath=387
nasm -f elf32 syscall.s -o ../build/syscall.o
gcc -c bosystd.c -o ../build/bosystd.o -m32 -static -fno-toplevel-reorder -mgeneral-regs-only -ffreestanding -nostdlib -mhard-float -mfpmath=387
cd ..
ld -T link.ld -o usercode.o build/* -melf_i386
objcopy -O binary usercode.o usercode.bin
objcopy --dump-section .bossec=usercode.bin.bossec usercode.o