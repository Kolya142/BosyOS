# Usage
## first run
**install qemu**
```bash
make releaserun
```
## build all and run
```bash
make all
```
## build kernel-only
```bash
make kernel
```
## build kernel-only and run
```bash
make kernelrun
```
## comple bootloader and kernel to one file
```bash
make compile
```
# Components
## bootsegment
Monolithic x86 bootloader \
Requirements: 0x79c5 magic number
## Asciip
[Ascii+](ascii+.txt)
# Thirdparty
[Vga](vga.md)