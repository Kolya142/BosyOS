i = open("ser.in", 'w')
o = open("ser.out", 'r')

while True:
    print(o.read(1024))
    c = input()
    o.write(c)