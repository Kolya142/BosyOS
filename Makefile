
.PHONY: boot kernel compile run all kernelrun loadfromrelease releaseerun iso
boot:
	cd bootloader; nasm main.asm
	cd kbootloader; nasm -f bin boot.asm -o boot
	cat bootloader/main kbootloader/boot > bootsegment
	python3 non-kernel\ files/bosybuild.py initram.asm N && truncate -s 4096 initram.bsf
kernel:
	cd kernel && python3 build.py && truncate -s 31564 kernel.b
compile:
	cat bootsegment initram.bsf kernel/kernel.b > drive
run:
	qemu-system-i386 -drive format=raw,file=drive -display gtk,zoom-to-fit=on -m 64M -audiodev pa,id=snd0 -machine pcspk-audiodev=snd0 --enable-kvm
all: boot kernel compile run
kernelrun: kernel compile run
load: img
	sudo bash load.sh
loadfromrelease:
	cp release/* .
releaserun: loadfromrelease run
img:
	echo -e '\n\n\n---BosyOS.img--- Link: https://github.com/Kolya142/BosyOS' | cat drive - > BosyOS.img