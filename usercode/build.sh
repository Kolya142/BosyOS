mkdir -p build
rm -f build/*
set -e
cd code
nasm main.s -o ../test.o -felf32
ld -m elf_i386 -Ttext=0x08048000 -o ../../userdir/bin/test2.elf ../test.o

nasm syscall.s -o ../build/syscall.o -f elf32
gcc bosyos/bosystd.c -o ../build/std.o -fno-plt -fno-pic -fno-pie -static -fno-toplevel-reorder -mgeneral-regs-only -ffreestanding -m32 -mhard-float -nostartfiles -mfpmath=387 -c
gcc main.c -o ../build/usercode.o -fno-plt -fno-pic -fno-pie -static -fno-toplevel-reorder -mgeneral-regs-only -ffreestanding -m32 -mhard-float -nostartfiles -mfpmath=387 -c
ld -m elf_i386 -Ttext=0x08048000 -o ../../userdir/bin/init.elf ../build/*
cd ..