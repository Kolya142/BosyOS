SYSTEM=linux

ifeq ($(shell uname),Darwin)
  SYSTEM=macos
endif

.PHONY: kernel compile run all kernelrun loadfromrelease releaseerun userdata_dump prog progc progrun
kernel:
	cd kernel/boot && nasm boot.asm -o boota.bin && gcc -x c -c -m32 boot._c -o bootc.o -static -fno-toplevel-reorder -mgeneral-regs-only -ffreestanding -m32
	cd kernel/boot && ld -m elf_i386 -T link.ld bootc.o -o bootc.oo
	cd kernel/boot && objcopy -O binary bootc.oo bootc.bin
	cd kernel/boot && cat boota.bin bootc.bin > boot.bin
	cd kernel/boot && truncate -s 1536 boot.bin
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
	truncate -s 24576 usercode/usercode.bin.bsf
compile:
	@if [ -e drive ]; then \
		make userdata_dump; \
	elif [ ! -e userdata ]; then \
 		truncate -s 1048576 userdata; \
	fi
	dd if=kernel/boot/boot.bin bs=512 of=drive seek=0
	dd if=kernel.bin of=drive bs=512 seek=35
	dd if=userdata of=drive bs=512 seek=291

QEMU=qemu-system-i386
QEMU_DRIVE=-drive format=raw,file=drive
QEMU_MOUSE=
QEMU_MEM=-m 64M
QEMU_SER=-serial stdio
QEMU_NET=
QEMU_DEBUG=-D ~/bosyos.qemu.log -d int,cpu_reset --no-reboot --no-shutdown
QEMU_AUDIO_LINUX=-audiodev pa,id=snd0 -machine pcspk-audiodev=snd0
QEMU_AUDIO_MAC=-audiodev coreaudio,id=snd0 -machine pcspk-audiodev=snd0
QEMU_DISPLAY=-display gtk,zoom-to-fit=on
QEMU_KVM=--enable-kvm
QEMU_GDB=-s -S
QEMU_OUT=

run:
	@echo -e "\n\n\n\n\n\n\n\n\n\n\n\n"
	@if [ "$(MODE)" = "min" ]; then \
		$(QEMU) $(QEMU_DRIVE) $(QEMU_MEM) $(QEMU_SER) $(QEMU_MOUSE) $(QEMU_OUT); \
	elif [ "$(MODE)" = "debug" ]; then \
		$(QEMU) $(QEMU_DRIVE) $(QEMU_MEM) $(QEMU_SER) $(QEMU_DEBUG) $(QEMU_MOUSE) $(QEMU_OUT); \
	elif [ "$(MODE)" = "gdb" ]; then \
		$(QEMU) $(QEMU_DRIVE) $(QEMU_MEM) $(QEMU_SER) $(QEMU_GDB) $(QEMU_MOUSE) $(QEMU_OUT); \
	elif [ "$(SYSTEM)" = "macos" ]; then \
		$(QEMU) $(QEMU_DRIVE) $(QEMU_MEM) $(QEMU_NET) $(QEMU_SER) $(QEMU_AUDIO_MAC) $(QEMU_MOUSE) $(QEMU_OUT); \
	else \
		$(QEMU) $(QEMU_DRIVE) $(QEMU_MEM) $(QEMU_NET) $(QEMU_SER) $(QEMU_DISPLAY) $(QEMU_AUDIO_LINUX) $(QEMU_KVM) $(QEMU_MOUSE) $(QEMU_OUT) --no-reboot --no-shutdown;\
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