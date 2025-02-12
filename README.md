# Kernel development
## Warning
### global variables
in header: \
`extern I16 variable;` \
in source: \
`I16 variable = 4;`
## NameStyle
### local variable
`customer_data` snake_case
### global variable driver
`vga` snake_case
### global variable
`TTYCursor` SYSCase
### global function
`TTYPrint` SYSCase
### global function driver
`inb` snake_case
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