import struct
import sys

if len(sys.argv) < 2:
    print(f"Usage: {sys.argv[0]} <DAT dump>")
    sys.exit(1)

dat = None
with open(sys.argv[1], "rb") as f:
    dat = f.read()

sectors = len(dat) // 512

if sectors == 0 or sectors * 512 != len(dat):
    print(f"Invalid file: sectors {sectors}, len {len(dat)}, len / sectors {len(dat) / max(1, sectors)}")
    sys.exit(1)

print("Sector count:", sectors)
print("Magic:", dat[:2])
tal = dat[2:][:(sectors//8)]
tal = ''.join(bin(i)[2:].zfill(8)[::-1] for i in struct.unpack("<" + "B" * len(tal), tal))[1:]
tid = struct.unpack("<" + "H" * sectors, dat[2+(sectors//8):][:(sectors*2)])[1:]
print("Alloc Table:", tal, len(tal))
print("ID Table:", '(' + ' '.join(hex(i)[2:] for i in tid) + ')', len(tid))
print("Sector List:")

for i in range(sectors-1):
    if tal[i] != '1':
        continue
    print(f" Sector {hex(i)[2:]} - ID {hex(tid[i])[2:]}h")

while True:
    d = input("Sector Index for Dump(? - exit): ")
    if d == '?':
        break
    d = int(d)
    if d < 0 or d > sectors - 1:
        print("Invalid Sector Index")
        continue
    lz = 511
    for i in range(511, 0, -1):
        if dat[512 + d * 512 + i] != 0:
            break
        lz = i
    for i in range(0, lz+1, 16):
        chunk = dat[512 + d * 512 + i : 512 + d * 512 + min(i + 16, lz + 1)]
        hex_data = ' '.join(f"{b:02x}" for b in chunk)
        ascii_data = ''.join((f'\x1b[32m{chr(b)}\x1b[0m' if b != ord(' ') else f'\x1b[34m.\x1b[0m') if 0x20 <= b < 0x80 else ('\x1b[31m.\x1b[0m' if b == 0 else '\x1b[34m.\x1b[0m') for b in chunk)
        print(f"{i:04x}: {hex_data.ljust(47)} | {ascii_data}")
    if lz + 1 != 512:
        print('... (zeroes)', end='')
    print()