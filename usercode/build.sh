mkdir -p build
rm -f build/*
set -e
cd code
nasm main.s -o ../build/usercode.o -felf32
ld -m elf_i386 -T ../link.ld -o ../usercode.o ../build/*
objcopy -O binary ../usercode.o ../usercode.bin
objcopy --dump-section .bossec=../usercode.bin.bossec ../usercode.o
cd ..