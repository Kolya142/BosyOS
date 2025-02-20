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
## Vga
[VGACodePage437](https://en.wikipedia.org/wiki/Code_page_437)
### Colors
0 - Black \
1 - DBlue \
2 - DGreen \
3 - DCyan \
4 - DRed \
5 - DPurple \
6 - Brown \
7 - Gray \
8 - DGray \
9 - Blue \
A - Green \
B - Cyan \
C - Red \
D - Purple \
E - Yellow \
F - White