code = "ХЪЖЭБЮЕ" # [];',.`
alhp = "АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ"
AC = 0xB1

for c in code:
    print(f'\\x{alhp.find(c)+AC:02x}', end='')
print()