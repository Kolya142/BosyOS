form = """
unsigned char TTYFont[256][5] = {
    [' '] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000},
    ['!'] = {0b00100, 0b00100, 0b00100, 0b00000, 0b00100},
    ['"'] = {0b10100, 0b10100, 0b00000, 0b00000, 0b00000},
    ['#'] = {0b01010, 0b11111, 0b01010, 0b11111, 0b01010},
    ['$'] = {0b00100, 0b01100, 0b01110, 0b00110, 0b00100},
    ['%'] = {0b10001, 0b00010, 0b00100, 0b01000, 0b10001},
    ['&'] = {0b01100, 0b10000, 0b01101, 0b10010, 0b01101},
    ['\\\''] = {0b00010, 0b00010, 0b00000, 0b00000, 0b00000},
    ['('] = {0b00100, 0b01000, 0b01000, 0b01000, 0b00100},
    [')'] = {0b00100, 0b00010, 0b00010, 0b00010, 0b00100},
    ['*'] = {0b01100, 0b01100, 0b00000, 0b00000, 0b00000},
    ['+'] = {0b00000, 0b00100, 0b01110, 0b00100, 0b00000},
    ['-'] = {0b00000, 0b00000, 0b01110, 0b00000, 0b00000},
    [','] = {0b00000, 0b00000, 0b00000, 0b00100, 0b01000},
$$
};
"""

text = ""

for i in range(0x2E, 0x7F):
    c = chr(i)
    char = open(f"chars/{c}.txt").read()
    lines = char.strip().split("\n")
    
    text += f"    ['{c.replace('\\', '\\\\').replace('\'', '\\\'')}'] = " + '{'
    
    bit_lines = [f"0b{line.replace('.', '0').replace('a', '1')}" for line in lines]
    text += ", ".join(bit_lines)
    
    text += '},\n'

for i in range(0x80, 0xFF):
    char = open(f"chars/{hex(i)[2:]}.txt").read()
    lines = char.strip().split("\n")
    
    text += f"    [0x{hex(i)[2:]}] = " + '{'
    
    bit_lines = [f"0b{line.replace('.', '0').replace('a', '1')}" for line in lines]
    text += ", ".join(bit_lines)
    
    text += '},\n'

with open("font.c", 'w') as f:
    f.write(form.replace('$$', text))
