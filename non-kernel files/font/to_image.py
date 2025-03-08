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

text = "the quick brown fox jumps over a lazy dog. THE QUICK BROWN FOX JUMPS OVER A LAZY DOG. \xc0\xc1\xb9\xb3\xb6\xc3 \xbd\xb9\xc1 $echo 123 \\//##@!$%^&*()_+-=~`"
color = (0, 255, 255)

img = np.zeros((5, len(text)*6, 3))

for i, c in enumerate(text):
    if c not in font:
        continue
    char = np.zeros((5, 5, 3))
    for j in range(25):
        char[j//5,j%5,0] = color[0] if font[c][j] == 'a' else 0
        char[j//5,j%5,1] = color[1] if font[c][j] == 'a' else 0
        char[j//5,j%5,2] = color[2] if font[c][j] == 'a' else 0
    x = i * 6
    img[:, x:x + 5] = char

cv2.imwrite("text.png", img)