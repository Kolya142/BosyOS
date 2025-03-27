import numpy as np
import cv2

font = {}

for i in range(0x21, 0x7F):
    c = chr(i)
    char = open(f"chars/{c.replace('/', 'front slash')}.txt").read()
    font[c] = char.replace('\n', '')

for i in range(0x80, 0xFF):
    c = chr(i)
    char = open(f"chars/{i:2x}.txt").read()
    font[c] = char.replace('\n', '')

text = "".join(chr(i) for i in range(256))

img = np.zeros((len(text)*9//16, len(text)*9//16, 3))

colors = [
    (0x00, 0x00, 0xFF),
    (0x00, 0xFF, 0x00),
    (0x00, 0xFF, 0xFF),
    (0xFF, 0x00, 0x00),
    (0xFF, 0x00, 0xFF),
    (0xF5, 0xD4, 0x00),
    (0xFF, 0xFF, 0xFF),
]

for i, c in enumerate(text):
    if c not in font:
        continue
    print(c)
    for j in range(8*8):
        if font[c][j] == 'a':
            img[j//8+(i//16)*9,j%8+(i%16)*9] = (255, 255, 255)
    x = i * 8

cv2.imwrite("text.png", img)