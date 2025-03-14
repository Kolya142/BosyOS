
rm build/* -r
set -e
cd code
i686-elf-gcc -c main.c -o ../build/main.o -m32 -static -fno-toplevel-reorder -mgeneral-regs-only -ffreestanding -nostdlib -mfloat-abi=hard -mfpmath=387
nasm -f elf32 syscall.s -o ../build/syscall.o
i686-elf-gcc -c bosystd.c -o ../build/bosystd.o -m32 -static -fno-toplevel-reorder -mgeneral-regs-only -ffreestanding -nostdlib -mfloat-abi=hard -mfpmath=387
cd ..
i686-elf-ld -T link.ld -o usercode.o build/*
i686-elf-objcopy -O binary usercode.o usercode.bin
i686-elf-objcopy --dump-section .bossec=usercode.bin.bossec usercode.o