import os

os.system("mkdir chars -p")

for i in range(0x21, 0x7F):
    if os.path.exists(f"chars/{chr(i).replace('/', 'front slash')}.txt"): continue
    with open(f'chars/{chr(i).replace('/', 'front slash')}.txt', 'w') as f:
        f.write("........\n" * 8)

for i in range(0x80, 0xFF):
    if os.path.exists(f"chars/{hex(i)[2:]}.txt"): continue
    with open(f'chars/{hex(i)[2:]}.txt', 'w') as f:
        f.write("........\n" * 8)