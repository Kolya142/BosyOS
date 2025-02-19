import sys

argv = sys.argv
argc = len(argv)

if argc != 2:
    print(f"Usage: {argv[0]} <binary file>")
    sys.exit(1)

data = open(argv[1], 'rb').read()
output = []

for b in data:
    if b == 0:
        output.append("\\0")
    elif 0x20 <= b <= 0x79:
        output.append(chr(b))
    elif b == 0x0A:
        output.append("\\n")
    elif b == 0x0D:
        output.append("\\r")
    elif b == ord('\t'):
        output.append("\\t")
    elif b == ord('\b'):
        output.append("\\b")
    else:
        output.append(f"\\x{b:02x}")

print(''.join(output))
