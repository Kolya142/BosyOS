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

QEMU=qemu-system-i386
QEMU_DRIVE=-drive format=raw,file=drive
QEMU_MEM=-m 64M
QEMU_SER=-serial stdio
QEMU_NET=-netdev bridge,id=net0,br=br0 -device rtl8139,netdev=net0
QEMU_DEBUG=-D ~/bosyos.qemu.log -d int,cpu_reset --no-reboot --no-shutdown
QEMU_AUDIO_LINUX=-audiodev pa,id=snd0 -machine pcspk-audiodev=snd0
QEMU_AUDIO_MAC=-audiodev coreaudio,id=snd0 -machine pcspk-audiodev=snd0
QEMU_DISPLAY=-display gtk,zoom-to-fit=on
QEMU_KVM=--enable-kvm
QEMU_GDB=-s -S
QEMU_OUT=

run:
	rm -f ser.in
	rm -f ser.out
	mkfifo ser.in ser.out
	@echo -e "\n\n\n\n\n\n\n\n\n\n\n\n"
	@if [ "$(MODE)" = "min" ]; then \
		$(QEMU) $(QEMU_DRIVE) $(QEMU_MEM) $(QEMU_SER) $(QEMU_OUT); \
	elif [ "$(MODE)" = "debug" ]; then \
		$(QEMU) $(QEMU_DRIVE) $(QEMU_MEM) $(QEMU_SER) $(QEMU_DEBUG) $(QEMU_OUT); \
	elif [ "$(MODE)" = "gdb" ]; then \
		$(QEMU) $(QEMU_DRIVE) $(QEMU_MEM) $(QEMU_SER) $(QEMU_GDB) $(QEMU_OUT); \
	elif [ "$(SYSTEM)" = "macos" ]; then \
		$(QEMU) $(QEMU_DRIVE) $(QEMU_MEM) $(QEMU_NET) $(QEMU_SER) $(QEMU_AUDIO_MAC) $(QEMU_OUT); \
	else \
		$(QEMU) $(QEMU_DRIVE) $(QEMU_MEM) $(QEMU_NET) $(QEMU_SER) $(QEMU_DISPLAY) $(QEMU_AUDIO_LINUX) $(QEMU_KVM) $(QEMU_OUT); \
	fi
	@make userdata_dump

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
driveget:
	curl https://raw.githubusercontent.com/Kolya142/BosyOS/refs/heads/main/release/drive -o drive