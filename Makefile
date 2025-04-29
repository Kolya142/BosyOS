.PHONY: kernel all run loader compile

kernel:
	python3 build.py
compile:
	cd grub; bash build.sh
run:
	qemu-system-i386 grub/bosyos2.iso --enable-kvm -display sdl,gl=on
all: kernel compile run
