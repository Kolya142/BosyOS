form = """
unsigned char TTYFont[256][5] = {
$$
};
"""

text = ""

for i in range(0x21, 0x7F):
    c = chr(i)
    char = open(f"chars/{c.replace('/', 'front slash')}.txt").read()
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
