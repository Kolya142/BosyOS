from pathlib import Path
import struct
import sys
import os

if len(sys.argv) != 3:
    print(f"usage: {sys.argv[0]} <OUTPUT FILE> <INPUT DIRECTORY>")

with open(sys.argv[1], 'wb') as f:
    f.write(struct.pack('<7s', b'-romfs-') + struct.pack('<I', len(os.listdir(sys.argv[2]))))
    for file in os.listdir(sys.argv[2]):
        buf = bytearray()
        buf.extend(file.encode('ascii')+b'\0'*(12-len(file)))
        with open(Path(sys.argv[2]) / file, 'rb') as f1:
            content = f1.read()
            buf.extend(struct.pack('<I', len(content)))
            buf.extend(content)
        f.write(buf)
        print(buf)