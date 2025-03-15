import sys

addrs = []

for line in sys.stdin:
    addrs.append(int(line, base=0))

def ff(i):
    l = i.split()
    if len(l) < 4:
        return None
    return l[3], l[1]

with open("kernel/func-dump.txt", 'r') as f:
    funcs = [ff(i) for i in f.read().split('\n')]
    for i in funcs:
        if int(i[0], base=16) in addrs:
            print(i[0], '->', i[1])