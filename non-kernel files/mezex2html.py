mez = open("bosyos.mez").read()
html = ""

html += "<!DOCTYPE html>\n"
html += "<html>\n"
html += "<head>\n"
html += "<style>\n"
html +=""".Q0 {
color: #000000;
}
.Q1 {
color: #000055;
}
.Q2 {
color: #005500;
}
.Q3 {
color: #005555;
}
.Q4 {
color: #550000;
}
.Q5 {
color: #550055;
}
.Q6 {
color: #554400;
}
.Q7 {
color: #565656;
}
.Q8 {
color: #444444;
}
.Q9 {
color: #0000ff;
}
.QA {
color: #00ff00;
}
.QB {
color: #00ffff;
}
.QC {
color: #ff0000;
}
.QD {
color: #ff00ff;
}
.QE {
color: #f5d400;
}
.QF {
color: #ffffff;
}

.K0 {
background-color: #000000;
}
.K1 {
background-color: #000055;
}
.K2 {
background-color: #005500;
}
.K3 {
background-color: #005555;
}
.K4 {
background-color: #550000;
}
.K5 {
background-color: #550055;
}
.K6 {
background-color: #554400;
}
.K7 {
background-color: #565656;
}
.K8 {
background-color: #444444;
}
.K9 {
background-color: #0000ff;
}
.KA {
background-color: #00ff00;
}
.KB {
background-color: #00ffff;
}
.KC {
background-color: #ff0000;
}
.KD {
background-color: #ff00ff;
}
.KE {
background-color: #f5d400;
}
.KF {
background-color: #ffffff;
}
* {
overflow: hidden;
font-family: monospace, monospace;
}
text {
white-space: pre;
padding: 0;
}
"""
html += "</style>\n"
html += "</head>\n"
html += "<body class=\"K0\">\n"
width = 150
height = 90
vga  = list(" " * width * height)
vgac = [0x0F for _ in range(width*height)]
col = 0x0F
ttyc = 0
lbs = False
lds = False
ldv = 0
lbd = 0
lbp = 0
link = ""
for c in mez:
    print(lbs, lds, ldv, lbd, lbp, c)
    if lbs:
        if c == '\\':
            vga[ttyc] = c
            vgac[ttyc] = col
            ttyc += 1
        elif c == 'n':
            ttyc += width
            ttyc -= ttyc%width
        elif c == '$':
            vga[ttyc] = c
            vgac[ttyc] = col
            ttyc += 1
        lbs = False
    elif lds:
        if ldv == 3:
            if c != '~':
                link += c
                continue
            vga[ttyc] = f'<text onclick="window.location = \'{link}\';">{link}</text>'
            vgac[ttyc] = col
            ttyc += 1
            ldv = 0
            lds = False
            link = ''
        elif c == '!':
            ldv = 1
        elif c == '*':
            ldv = 2
        elif c == '^':
            ldv = 3
        elif c == 'U':
            ttyc -= width
            lds = 0
        elif c == 'D':
            ttyc += width
            lds = 0
        elif c == 'L':
            ttyc -= 1
            lds = 0
        elif c == 'R':
            ttyc += 1
            lds = 0
        elif c == 'H':
            ttyc = 0
            lds = 0
        elif c == 'x':
            lbp = 1
        elif lbp:
            lbd <<= 4
            lbd |= int(c, 16)
            lbp += 1
            if lbp == 3:
                vga[ttyc] = lbd
                vgac[ttyc] = col
                ttyc += 1
                lbd = 0
                lds = False
                lbp = 0
        else:
            if ldv == 1:
                d = int(c, 16)
                col = col & 0xf0 | d
            elif ldv == 2:
                d = int(c, 16)
                col = col & 0x0f | (d<<4)
            ldv = 0
            lds = False
    elif c == '\\':
        lbs = True
    elif c == '\n':
        ttyc += width
        ttyc -= ttyc%width
    elif c == '$':
        lds = True
    else:
        vga[ttyc] = c
        vgac[ttyc] = col
        ttyc += 1

i = 0
last_s = 3333
html += "<text>"
for c, s in zip(vga, vgac):
    if i % width == 0 and i != 0:
        html += '<br>'
    if s != last_s:
        html += f'</text>'
        print(hex(s))
        html += f"<text class=\"K{hex(s>>4)[2:][:1].upper()} Q{hex(s&15)[2:][:1].upper()}\">{c}"
    else:
        html += c
    last_s = s
    i += 1
html += "</body>\n"
html += "</html>\n"

open("bosyos.html", 'w').write(html)