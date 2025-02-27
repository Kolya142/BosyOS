SYSTEM=linux

ifeq ($(shell uname),Darwin)
  SYSTEM=macos
endif

.PHONY: boot kernel compile run all kernelrun loadfromrelease releaseerun userdata_dump prog progc progrun
boot:
	cd bootloader; nasm main.asm
	cd kbootloader; nasm -f bin boot.asm -o boot
	cat bootloader/main kbootloader/boot > bootsegment
kernel:
	cd kernel && python3 build.py && truncate -s 65536 kernel.b  # 64 KB
userdata_dump:
	@if [ -e drive ]; then \
		dd if=drive of=userdata bs=512 skip=3 count=32; \
	else \
		echo "Error: drive file not found!"; \
	fi
prog:
	cd usercode && bash build.sh && python3 bsfexe.py usercode.bin N
	truncate -s 24576 usercode/usercode.bin.bsf
compile:
	@if [ -e drive ]; then \
		make userdata_dump; \
	elif [ ! -e userdata ]; then \
 		truncate -s 16384 userdata; \
	fi
	cat bootsegment userdata kernel/kernel.b usercode/usercode.bin.bsf > drive

run:
	@if [ $(MODE) = "min" ]; then \
		qemu-system-i386 -drive format=raw,file=drive -m 64M; \
	elif [ $(SYSTEM) = "macos" ]; then \
		qemu-system-i386 -drive format=raw,file=drive -m 64M -audiodev coreaudio,id=snd0 -machine pcspk-audiodev=snd0; \
	else \
		qemu-system-i386 -drive format=raw,file=drive -display gtk,zoom-to-fit=on -m 64M -audiodev pa,id=snd0 -machine pcspk-audiodev=snd0 --enable-kvm; \
	fi && [ $$? -eq 0 ] && make userdata_dump

all: boot kernel prog compile run
allr: boot kernel prog compile
progc: prog compile
progrun: progc run
kernelc: kernel compile
kernelrun: kernel compile run
load: img
	sudo bash load.sh
loadfromrelease:
	cp release/* .
releaserun: loadfromrelease run