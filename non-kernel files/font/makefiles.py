import os

os.system("mkdir chars")

for i in range(0x2E, 0x7F):
    with open(f'chars/{chr(i)}.txt', 'w') as f:
        f.write(".....\n.....\n.....\n.....\n.....\n")