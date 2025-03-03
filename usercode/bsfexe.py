import os
import sys

STACK = 10*4

argv = sys.argv
argc = len(argv)

if argc != 3:
    print(f"Usage: {argv[0]} <source code> <Y/N compress>")
    sys.exit(1)

if argv[2] not in 'YN':
    print(f"Usage: {argv[0]} <source code> <Y/N compress>")
    sys.exit(1)

builded = argv[1]
build = bytearray()
meta = bytearray()
with open(builded, 'rb') as f:
    build.extend(f.read())
with open(builded+'.bossec', 'rb') as f:
    meta.extend(f.read())
code_len = len(build)
build = b'BOSY' + code_len.to_bytes(4, 'little', signed=False) + len(meta).to_bytes(4, 'little', signed=False) + build + meta
print("Header:", build[:8])
print("Build size(Full):", len(build))
with open(builded+'.bsf', 'wb') as f:
    f.write(build)
print(build)
if argv[2] == 'Y':
    os.system(f"bosz {builded}")