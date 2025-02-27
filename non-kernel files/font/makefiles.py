import os

os.system("mkdir chars")

for i in range(0x2E, 0x7F):
    if os.path.exists(f"chars/{chr(i)}.txt"): continue
    with open(f'chars/{chr(i)}.txt', 'w') as f:
        f.write(".....\n.....\n.....\n.....\n.....\n")

for i in range(0x80, 0xFF):
    if os.path.exists(f"chars/{hex(i)[2:]}.txt"): continue
    with open(f'chars/{hex(i)[2:]}.txt', 'w') as f:
        f.write(".....\n.....\n.....\n.....\n.....\n")