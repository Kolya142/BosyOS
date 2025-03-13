SYSTEM=linux

ifeq ($(shell uname),Darwin)
  SYSTEM=macos
endif

.PHONY: kernel compile run all kernelrun loadfromrelease releaseerun userdata_dump prog progc progrun iso
kernel:
	cd kernel && python3 build.py
	cp kernel/kernel.b kernel.bin
	truncate -s 131072 kernel.bin
userdata_dump:
	# @if [ -e drive ]; then \
	# 	dd if=drive of=userdata bs=512 skip=291 count=2048; \
	# else \
	# 	echo "Error: drive file not found!"; \
	# fi
prog:
	cd usercode && bash build.sh && python3 bsfexe.py usercode.bin N
compile:
	truncate -s 181760 drive
	# @if [ -e drive ]; then \
	# 	make userdata_dump; \
	# elif [ ! -e userdata ]; then \
 	# 	truncate -s 181760 userdata; \
	# fi
	dd if=bosyboot of=drive bs=512 seek=0 count=13 conv=notrunc
	dd if=kernel.bin of=drive bs=512 seek=35 count=256 conv=notrunc

iso:
	touch bosyos.iso
	sudo losetup /dev/loop1 bosyos.iso
	sudo dd if=drive of=/dev/loop1p1
	sudo losetup -d /dev/loop1

QEMU=qemu-system-i386
QEMU_DRIVE=-drive format=raw,file=drive
QEMU_MOUSE=
QEMU_MEM=-m 64M
QEMU_SER=-serial stdio
QEMU_NET=-device rtl8139,netdev=net0 -netdev user,id=net0
QEMU_DEBUG=-D ~/bosyos.qemu.log -d int,cpu_reset --no-reboot --no-shutdown
QEMU_AUDIO_LINUX=-audiodev pa,id=snd0 -machine pcspk-audiodev=snd0
QEMU_AUDIO_MAC=-audiodev coreaudio,id=snd0 -machine pcspk-audiodev=snd0
QEMU_DISPLAY=-display gtk,zoom-to-fit=on
QEMU_KVM=--enable-kvm
QEMU_USB=-device piix3-usb-uhci,id=usb
QEMU_GDB=-s -S
QEMU_OUT=

run:
	@echo -e "\n\n\n\n\n\n\n\n\n\n\n\n"
	@if [ "$(MODE)" = "min" ]; then \
		$(QEMU) $(QEMU_ADD) $(QEMU_DRIVE) $(QEMU_USB) $(QEMU_MEM) $(QEMU_SER) $(QEMU_OUT); \
	elif [ "$(MODE)" = "debug" ]; then \
		$(QEMU) $(QEMU_ADD) $(QEMU_DRIVE) $(QEMU_USB) $(QEMU_MEM) $(QEMU_SER) $(QEMU_DEBUG) $(QEMU_MOUSE) $(QEMU_OUT); \
	elif [ "$(MODE)" = "gdb" ]; then \
		$(QEMU) $(QEMU_ADD) $(QEMU_DRIVE) $(QEMU_USB) $(QEMU_MEM) $(QEMU_SER) $(QEMU_GDB) $(QEMU_MOUSE) $(QEMU_OUT); \
	elif [ "$(SYSTEM)" = "macos" ]; then \
		$(QEMU) $(QEMU_ADD) $(QEMU_DRIVE) $(QEMU_USB) $(QEMU_MEM) $(QEMU_NET) $(QEMU_SER) $(QEMU_AUDIO_MAC) $(QEMU_MOUSE) $(QEMU_OUT); \
	else \
		$(QEMU) $(QEMU_ADD) $(QEMU_DRIVE) $(QEMU_USB) $(QEMU_MEM) $(QEMU_NET) $(QEMU_SER) $(QEMU_DISPLAY) $(QEMU_AUDIO_LINUX) $(QEMU_KVM) $(QEMU_MOUSE) $(QEMU_OUT) --no-reboot --no-shutdown;\
	fi
	@make userdata_dump

all: kernel prog compile run
allr: kernel prog compile
progc: prog compile
progrun: progc run
kernelc: kernel compile
kernelrun: kernel compile run
load: img
	sudo bash load.sh
loadfromrelease:
	cp release/* .
releaserun: loadfromrelease run
driveget:
	curl https://raw.githubusercontent.com/Kolya142/BosyOS/refs/heads/main/release/drive -o drive