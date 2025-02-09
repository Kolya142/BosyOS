.PHONY: boot kernel compile run all kernelrun
boot:
	cd bootloader; nasm main.asm
	cd kernel/boot; nasm -f bin boot.asm -o boot
	cat bootloader/main kernel/boot/boot > bootsegment
kernel:
	cd kernel; python3 build.py; truncate -s 31564 kernel.b
compile:
	cat bootsegment kernel/kernel.b > drive
run:
	qemu-system-i386 -drive format=raw,file=drive -icount 5,align=on -display gtk,zoom-to-fit=on -m 9M
all: boot kernel compile run
kernelrun: kernel compile run
load: 
	sudo bash load.sh