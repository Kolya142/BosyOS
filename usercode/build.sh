mkdir -p build
rm -f build/*
set -e
cd code
nasm syscall.s -o ../build/syscall.o -f elf32
nasm bosyentry.s -o ../build/entry.o -f elf32
gcc bosyos/bosystd.c -o ../build/std.o -fno-plt -fno-pic -fno-pie -static -fno-toplevel-reorder -mgeneral-regs-only -ffreestanding -m32 -mhard-float -nostartfiles -mfpmath=387 -c
gcc main.c -o ../build/usercode.o -fno-plt -fno-pic -fno-pie -static -fno-toplevel-reorder -mgeneral-regs-only -ffreestanding -m32 -mhard-float -nostartfiles -mfpmath=387 -c
ld -m elf_i386 -T ../link.ld -o ../usercode.o ../build/*
ld -m elf_i386 -T ../link.elf.ld -o ../usercode.elf ../build/*
objcopy -O binary ../usercode.o ../usercode.bin
objcopy --dump-section .bossec=../usercode.bin.bossec ../usercode.o
cd ..