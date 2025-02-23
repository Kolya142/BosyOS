SYSTEM=linux

ifeq ($(shell uname),Darwin)
  SYSTEM=macos
endif

.PHONY: boot kernel compile run all kernelrun loadfromrelease releaseerun iso
boot:
	cd bootloader; nasm main.asm
	cd kbootloader; nasm -f bin boot.asm -o boot
	cat bootloader/main kbootloader/boot > bootsegment
kernel:
	cd kernel && python3 build.py && truncate -s 31564 kernel.b
compile:
	@if [ -e drive ]; then \
		dd if=drive of=userdata bs=512 skip=3 count=16; \
	else \
 		truncate -s 8192 userdata; \
	fi
	cat bootsegment userdata kernel/kernel.b > drive

run:
	@if [ $(SYSTEM) = "macos" ]; then \
		qemu-system-i386 -drive format=raw,file=drive -m 64M -audiodev coreaudio,id=snd0 -machine pcspk-audiodev=snd0; \
	else \
		qemu-system-i386 -drive format=raw,file=drive -display gtk,zoom-to-fit=on -m 64M -audiodev pa,id=snd0 -machine pcspk-audiodev=snd0 --enable-kvm; \
	fi
	dd if=drive of=userdata bs=512 skip=3 count=16

all: boot kernel compile run
allr: boot kernel compile
kernelrun: kernel compile run
load: img
	sudo bash load.sh
loadfromrelease:
	cp release/* .
releaserun: loadfromrelease run
img:
	echo -e '\n\n\n---BosyOS.img--- Link: https://github.com/Kolya142/BosyOS' | cat drive - > BosyOS.img