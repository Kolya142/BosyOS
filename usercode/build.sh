set -e
cd code
nasm syscall.s -o ../build/syscall.o -felf32
nasm main.s -o ../usercode.elf -felf32
cd ..