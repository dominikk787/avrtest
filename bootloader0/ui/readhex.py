from intelhex import IntelHex
import sys
import subprocess

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

data = ''

for i in range(0, iterations):
    start = i * 64
    print(start * 2)
    start0 = tohex(round(start / 256))
    start1 = tohex(start % 256)
    commandsetaddr = 'python serialcli.py ' + port + ' ' + str(speed) + ' "A' + start1 + start0 + ' "'
    #print(commandsetaddr)
    commandreadflash = 'python serialcli.py ' + port+ ' ' + str(speed) + ' "R0x000x80 "'
    #print(commandreadflash)
    setaddr = subprocess.getoutput(commandsetaddr)
    #print(setaddr)
    readflash = subprocess.getoutput(commandreadflash)
    data = data + readflash[2:-2]
    #print(readflash)
    
g0 = False
for i in range(0, size):
    e = i * 2
    d = data[e:(e+2)]
    d = int(d, base=16)
    g1 = False
    g = (data[e] == 'F') and (data[e+1] == 'F')
    for f in range(0, 16):
        if (e + f) >= len(data):
            break
        if data[(e + f)] != 'F':
            g1 = True
            break
    if not g0:
        g0 = not g
        g1 = False
    else:
        g0 = g1
    if g1:
        hex[i] = d

hex.write_hex_file(file, byte_count=32)
print(hex.addresses())
