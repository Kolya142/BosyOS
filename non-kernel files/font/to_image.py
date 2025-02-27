import numpy as np
import cv2

font = {}

for i in range(0x2E, 0x7F):
    c = chr(i)
    char = open(f"chars/{c}.txt").read()
    font[c] = char.replace('\n', '')

text = "The quick brown fox jumps over a lazy dog     Hello, World"
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