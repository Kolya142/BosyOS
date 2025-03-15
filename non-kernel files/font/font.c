
static volatile unsigned char TTYFont[256][5] = {
    ['!'] = {0b00100, 0b00100, 0b00100, 0b00000, 0b00100},
    ['"'] = {0b01010, 0b01010, 0b00000, 0b00000, 0b00000},
    ['#'] = {0b01010, 0b11111, 0b01010, 0b11111, 0b01010},
    ['$'] = {0b00100, 0b01100, 0b01110, 0b00110, 0b00100},
    ['%'] = {0b10001, 0b00010, 0b00100, 0b01000, 0b10001},
    ['&'] = {0b01100, 0b10010, 0b01101, 0b10010, 0b01101},
    ['\''] = {0b00100, 0b00100, 0b00000, 0b00000, 0b00000},
    ['('] = {0b00100, 0b01000, 0b01000, 0b01000, 0b00100},
    [')'] = {0b00100, 0b00010, 0b00010, 0b00010, 0b00100},
    ['*'] = {0b00000, 0b00100, 0b00000, 0b00000, 0b00000},
    ['+'] = {0b00000, 0b00100, 0b01110, 0b00100, 0b00000},
    [','] = {0b00000, 0b00000, 0b00000, 0b00100, 0b01000},
    ['-'] = {0b00000, 0b00000, 0b01110, 0b00000, 0b00000},
    ['.'] = {0b00000, 0b00000, 0b00000, 0b00110, 0b00110},
    ['/'] = {0b00001, 0b00010, 0b00100, 0b01000, 0b10000},
    ['0'] = {0b01110, 0b11001, 0b10101, 0b10011, 0b01110},
    ['1'] = {0b00010, 0b00110, 0b00010, 0b00010, 0b00010},
    ['2'] = {0b01110, 0b10001, 0b00001, 0b00010, 0b01111},
    ['3'] = {0b01110, 0b00001, 0b01110, 0b00001, 0b01110},
    ['4'] = {0b00100, 0b01100, 0b11111, 0b00100, 0b00100},
    ['5'] = {0b01111, 0b01000, 0b00110, 0b00001, 0b00110},
    ['6'] = {0b00110, 0b01000, 0b01110, 0b01001, 0b01110},
    ['7'] = {0b01111, 0b00001, 0b00010, 0b00100, 0b01000},
    ['8'] = {0b01110, 0b10001, 0b01110, 0b10001, 0b01110},
    ['9'] = {0b01111, 0b01001, 0b01111, 0b00001, 0b01111},
    [':'] = {0b00000, 0b00100, 0b00000, 0b00000, 0b00100},
    [';'] = {0b00100, 0b00000, 0b00000, 0b00100, 0b01000},
    ['<'] = {0b00000, 0b01000, 0b10000, 0b01000, 0b00000},
    ['='] = {0b00000, 0b01110, 0b00000, 0b01110, 0b00000},
    ['>'] = {0b00000, 0b00010, 0b00001, 0b00010, 0b00000},
    ['?'] = {0b00110, 0b01001, 0b00001, 0b00000, 0b00001},
    ['@'] = {0b01110, 0b10101, 0b10111, 0b10000, 0b01110},
    ['A'] = {0b00100, 0b01010, 0b01110, 0b01010, 0b01010},
    ['B'] = {0b11100, 0b10010, 0b11100, 0b10010, 0b11100},
    ['C'] = {0b01110, 0b10000, 0b10000, 0b10000, 0b01110},
    ['D'] = {0b11100, 0b10010, 0b10010, 0b10010, 0b11100},
    ['E'] = {0b11110, 0b10000, 0b11110, 0b10000, 0b11110},
    ['F'] = {0b11111, 0b10000, 0b11111, 0b10000, 0b10000},
    ['G'] = {0b01110, 0b10000, 0b10111, 0b10001, 0b01110},
    ['H'] = {0b10001, 0b10001, 0b11111, 0b10001, 0b10001},
    ['I'] = {0b01110, 0b00100, 0b00100, 0b00100, 0b01110},
    ['J'] = {0b00010, 0b00010, 0b00010, 0b00010, 0b01100},
    ['K'] = {0b10010, 0b10100, 0b11000, 0b10100, 0b10010},
    ['L'] = {0b10000, 0b10000, 0b10000, 0b10000, 0b11110},
    ['M'] = {0b11011, 0b10101, 0b10001, 0b10001, 0b10001},
    ['N'] = {0b11001, 0b10101, 0b10101, 0b10011, 0b10001},
    ['O'] = {0b01110, 0b10001, 0b10001, 0b10001, 0b01110},
    ['P'] = {0b11100, 0b10010, 0b11100, 0b10000, 0b10000},
    ['Q'] = {0b01100, 0b10010, 0b11010, 0b10110, 0b01110},
    ['R'] = {0b11100, 0b10010, 0b11100, 0b11000, 0b10100},
    ['S'] = {0b01110, 0b10000, 0b01110, 0b00001, 0b01110},
    ['T'] = {0b01110, 0b00100, 0b00100, 0b00100, 0b00100},
    ['U'] = {0b10001, 0b10001, 0b10001, 0b10001, 0b01110},
    ['V'] = {0b10001, 0b10001, 0b01010, 0b01010, 0b00100},
    ['W'] = {0b10001, 0b10001, 0b10101, 0b10101, 0b01010},
    ['X'] = {0b10001, 0b01010, 0b00100, 0b01010, 0b10001},
    ['Y'] = {0b10001, 0b01010, 0b00100, 0b00100, 0b00100},
    ['Z'] = {0b11111, 0b00010, 0b00100, 0b01000, 0b11111},
    ['['] = {0b11100, 0b10000, 0b10000, 0b10000, 0b11100},
    ['\\'] = {0b10000, 0b01000, 0b00100, 0b00010, 0b00001},
    [']'] = {0b00111, 0b00001, 0b00001, 0b00001, 0b00111},
    ['^'] = {0b00100, 0b01010, 0b00000, 0b00000, 0b00000},
    ['_'] = {0b00000, 0b00000, 0b00000, 0b00000, 0b01110},
    ['`'] = {0b10000, 0b01000, 0b00000, 0b00000, 0b00000},
    ['a'] = {0b00110, 0b00001, 0b00111, 0b01001, 0b00111},
    ['b'] = {0b10000, 0b11110, 0b10001, 0b10001, 0b11110},
    ['c'] = {0b00000, 0b01110, 0b10000, 0b10000, 0b01110},
    ['d'] = {0b00001, 0b00001, 0b00111, 0b01001, 0b00111},
    ['e'] = {0b00000, 0b01110, 0b11110, 0b10000, 0b01110},
    ['f'] = {0b00110, 0b01000, 0b11110, 0b01000, 0b01000},
    ['g'] = {0b00000, 0b00011, 0b00011, 0b00001, 0b01110},
    ['h'] = {0b01000, 0b01000, 0b01100, 0b01010, 0b01010},
    ['i'] = {0b00100, 0b00000, 0b00100, 0b00100, 0b00100},
    ['j'] = {0b00000, 0b00010, 0b00010, 0b00010, 0b01100},
    ['k'] = {0b10010, 0b10100, 0b11000, 0b11000, 0b10100},
    ['l'] = {0b00100, 0b00100, 0b00100, 0b00100, 0b00100},
    ['m'] = {0b00000, 0b11010, 0b10101, 0b10001, 0b10001},
    ['n'] = {0b00000, 0b11100, 0b10010, 0b10010, 0b10010},
    ['o'] = {0b00000, 0b01100, 0b10010, 0b10010, 0b01100},
    ['p'] = {0b00000, 0b11100, 0b10100, 0b11000, 0b10000},
    ['q'] = {0b00000, 0b01110, 0b01010, 0b00110, 0b00010},
    ['r'] = {0b00000, 0b01110, 0b01001, 0b01000, 0b01000},
    ['s'] = {0b00000, 0b00110, 0b00100, 0b00010, 0b00110},
    ['t'] = {0b00100, 0b01110, 0b00100, 0b00100, 0b00010},
    ['u'] = {0b00000, 0b01001, 0b01001, 0b01001, 0b00111},
    ['v'] = {0b00000, 0b10010, 0b10010, 0b01100, 0b01100},
    ['w'] = {0b00000, 0b10001, 0b10001, 0b10101, 0b01010},
    ['x'] = {0b00000, 0b01001, 0b00110, 0b00110, 0b01001},
    ['y'] = {0b00000, 0b01010, 0b00100, 0b01000, 0b10000},
    ['z'] = {0b00000, 0b01110, 0b00010, 0b00100, 0b01110},
    ['{'] = {0b00110, 0b00100, 0b01100, 0b00100, 0b00110},
    ['|'] = {0b00100, 0b00100, 0b00100, 0b00100, 0b00100},
    ['}'] = {0b01100, 0b00100, 0b00110, 0b00100, 0b01100},
    ['~'] = {0b00000, 0b01010, 0b10100, 0b00000, 0b00000},
    [0x80] = {0b10101, 0b01010, 0b10101, 0b01010, 0b10101},
    [0x81] = {0b10101, 0b01110, 0b11111, 0b01110, 0b10101},
    [0x82] = {0b11111, 0b11111, 0b11111, 0b11111, 0b11111},
    [0x83] = {0b00000, 0b01111, 0b01000, 0b01011, 0b01010},
    [0x84] = {0b00000, 0b11111, 0b00000, 0b11011, 0b01010},
    [0x85] = {0b00000, 0b11110, 0b00010, 0b11010, 0b01010},
    [0x86] = {0b01010, 0b11010, 0b00010, 0b11110, 0b00000},
    [0x87] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0x88] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0x89] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0x8a] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0x8b] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0x8c] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0x8d] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0x8e] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0x8f] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0x90] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0x91] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0x92] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0x93] = {0b01010, 0b01011, 0b01000, 0b01011, 0b01010},
    [0x94] = {0b01010, 0b11011, 0b00000, 0b11011, 0b01010},
    [0x95] = {0b01010, 0b11010, 0b00010, 0b11010, 0b01010},
    [0x96] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0x97] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0x98] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0x99] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0x9a] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0x9b] = {0b00101, 0b01010, 0b10100, 0b01010, 0b00101},
    [0x9c] = {0b10100, 0b01010, 0b00101, 0b01010, 0b10100},
    [0x9d] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0x9e] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0x9f] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xa0] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xa1] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xa2] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xa3] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xa4] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xa5] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xa6] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xa7] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xa8] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xa9] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xaa] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xab] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xac] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xad] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xae] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xaf] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xb0] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xb1] = {0b00100, 0b01010, 0b01110, 0b10001, 0b10001},
    [0xb2] = {0b11111, 0b10000, 0b11111, 0b10001, 0b11110},
    [0xb3] = {0b11110, 0b10001, 0b11110, 0b10001, 0b11110},
    [0xb4] = {0b11111, 0b10000, 0b10000, 0b10000, 0b10000},
    [0xb5] = {0b01110, 0b01010, 0b10010, 0b11111, 0b10001},
    [0xb6] = {0b11111, 0b10000, 0b11111, 0b10000, 0b11111},
    [0xb7] = {0b10101, 0b10101, 0b01110, 0b10101, 0b10101},
    [0xb8] = {0b11111, 0b00001, 0b11110, 0b00001, 0b11111},
    [0xb9] = {0b10001, 0b10011, 0b10101, 0b11001, 0b10001},
    [0xba] = {0b00100, 0b10011, 0b10101, 0b11001, 0b10001},
    [0xbb] = {0b10010, 0b10100, 0b11000, 0b10100, 0b10010},
    [0xbc] = {0b00011, 0b00101, 0b00101, 0b01001, 0b10001},
    [0xbd] = {0b11011, 0b10101, 0b10001, 0b10001, 0b10001},
    [0xbe] = {0b10001, 0b10001, 0b11111, 0b10001, 0b10001},
    [0xbf] = {0b01110, 0b10001, 0b10001, 0b10001, 0b01110},
    [0xc0] = {0b11111, 0b10001, 0b10001, 0b10001, 0b10001},
    [0xc1] = {0b11100, 0b10010, 0b11100, 0b10000, 0b10000},
    [0xc2] = {0b01110, 0b10000, 0b10000, 0b10000, 0b01110},
    [0xc3] = {0b11111, 0b00100, 0b00100, 0b00100, 0b00100},
    [0xc4] = {0b10001, 0b10001, 0b01010, 0b00100, 0b00100},
    [0xc5] = {0b00100, 0b01110, 0b10101, 0b01110, 0b00100},
    [0xc6] = {0b10001, 0b01010, 0b00100, 0b01010, 0b10001},
    [0xc7] = {0b10010, 0b10010, 0b10010, 0b11111, 0b00001},
    [0xc8] = {0b10001, 0b10001, 0b01111, 0b00001, 0b00001},
    [0xc9] = {0b10101, 0b10101, 0b10101, 0b10101, 0b11111},
    [0xca] = {0b10101, 0b10101, 0b10101, 0b11111, 0b00001},
    [0xcb] = {0b11000, 0b01000, 0b01110, 0b01001, 0b01110},
    [0xcc] = {0b10001, 0b10001, 0b11001, 0b10101, 0b11001},
    [0xcd] = {0b10000, 0b10000, 0b11100, 0b10010, 0b11100},
    [0xce] = {0b11110, 0b00001, 0b11111, 0b00001, 0b11110},
    [0xcf] = {0b10010, 0b10101, 0b11101, 0b10101, 0b10010},
    [0xd0] = {0b00110, 0b01001, 0b00111, 0b01001, 0b10001},
    [0xd1] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xd2] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xd3] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xd4] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xd5] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xd6] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xd7] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xd8] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xd9] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xda] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xdb] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xdc] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xdd] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xde] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xdf] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xe0] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xe1] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xe2] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xe3] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xe4] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xe5] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xe6] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xe7] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xe8] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xe9] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xea] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xeb] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xec] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xed] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xee] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xef] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xf0] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xf1] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xf2] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xf3] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xf4] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xf5] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xf6] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xf7] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xf8] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xf9] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xfa] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xfb] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xfc] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xfd] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    [0xfe] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},

};
