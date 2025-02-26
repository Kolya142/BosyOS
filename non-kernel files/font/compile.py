form = """
unsigned char TTYFont[256][5*5] = { // Reference: https://images.all-free-download.com/images/graphiclarge/pixelzim_3x5_6913975.jpg
$$
};
"""

text = ""

for i in range(0x2E, 0x7F):
    c = chr(i)
    char = open(f"chars/{c}.txt").read()
    text += f"    ['{c.replace('\\', '\\\\').replace('\'', '\\\'')}'] = " + '{\n        '
    text += char.replace('.', '0,').replace('a', '1,').replace('\n', '\n        ')
    text += '\n    },\n'

with open("font.c", 'w') as f:
    f.write(form.replace('$$', text))