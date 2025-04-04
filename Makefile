SYSTEM=linux

ifeq ($(shell uname),Darwin)
  SYSTEM=macos
endif

.PHONY: kernel compile run all kernelrun loadfromrelease releaseerun userdata_dump prog progc progrun iso
kernel:
	cd kernel && python3 build.py
	cp kernel/kernel.b kernel.bin
	truncate -s 131072 kernel.bin
_kernel_super_real_fast:
	cd kernel && python3 build.py --fast
	cp kernel/kernel.b kernel.bin
	truncate -s 131072 kernel.bin
kernel_super_real_fast:
	make _kernel_super_real_fast -j$(nproc)
prog:
	cd usercode && bash build.sh
compile:
	truncate -s 181760 drive
	# @if [ -e drive ]; then \
	# 	make userdata_dump; \
	# elif [ ! -e userdata ]; then \
 	# 	truncate -s 181760 userdata; \
	# fi
	non-kernel\ files/mkbosyrom initrom userdir
	cp initrom grub/iso/initrom
	cd grub && bash build.sh
	# dd if=bosyboot of=drive bs=512 seek=0 count=13 conv=notrunc
	# dd if=kernel.bin of=drive bs=512 seek=35 count=256 conv=notrunc
	# cp usercode/usercode.elf userdir/bin/test.elf
	# non-kernel\ files/mkbosyrom initrom userdir
	# dd if=initrom of=drive bs=512 seek=291 conv=notrunc

make_drive:
	rm /tmp/bosyos.drive || echo Creating new drive 
	truncate -s 2M /tmp/bosyos.drive
	cp /tmp/bosyos.drive grub/iso/drive

QEMU=qemu-system-i386
QEMU_DRIVE=-hda grub/bosyos.iso -drive file=drive,format=raw,if=ide,index=1
QEMU_MOUSE=
QEMU_MEM=-m 512M
QEMU_SER=-serial stdio
QEMU_NET=-device rtl8139,netdev=net0 -netdev user,id=net0
QEMU_DEBUG=-D ~/bosyos.qemu.log -d int,cpu_reset --no-reboot --no-shutdown
QEMU_AUDIO_LINUX=-audiodev pa,id=snd0 -machine pcspk-audiodev=snd0
QEMU_AUDIO_MAC=-audiodev coreaudio,id=snd0 -machine pcspk-audiodev=snd0
QEMU_DISPLAY=-display gtk,zoom-to-fit=on,gl=off
QEMU_KVM=--enable-kvm
QEMU_USB=-device piix3-usb-uhci,id=usb
QEMU_GDB=-s -S
QEMU_OUT=

run:
	truncate -s 2M drive
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

all: kernel compile run
allr: kernel compile
progc: prog compile
progrun: progc run
kernelc: kernel compile
kernelrun: kernel compile run
load:
	sudo $(QEMU) $(QEMU_ADD) $(QEMU_DRIVE) $(QEMU_USB) $(QEMU_MEM) $(QEMU_SER) $(QEMU_OUT) -drive file=/dev/sdb,format=raw,if=ide
shell:
	make prog
	cd userdir && bin/init.elf