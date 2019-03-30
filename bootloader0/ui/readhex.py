from intelhex import IntelHex
import sys
from bootclass import MyBoot

def tohex(data):
    h = format(data, 'X')
    if len(h) == 1:
        h = '0x0' + h
    else:
        h = '0x' + h
    return h


port = sys.argv[1]
speed = int(sys.argv[2])
file = sys.argv[3]
size = int(sys.argv[4], base = 16)

print(port)
print(speed)
print(file)
print(size)

iterations = round(size / 128)

hex = IntelHex()

data = []

boot = MyBoot(port, speed)
boot.open()

for i in range(0, iterations):
    start = i * 128
    print(start)
    boot.setAddr(start)
    print(boot.getAddr())
    datan = list(boot.readData(128))
    data = data + datan

boot.close()
    
g0 = True
for i in range(0, size):
    d = data[i]
    g1 = False
    g = (data[i] == 255)
    for f in range(0, 8):
        if (i + f) >= len(data):
            break
        if data[(i + f)] != 255:
            g1 = True
            break
    if not g0:
        g0 = not g
        g1 = False
    else:
        g0 = g1
    if not g:
        g1 = True
    if g1:
        hex[i] = d

hex.write_hex_file(file, byte_count=32)
print(hex.segments())
