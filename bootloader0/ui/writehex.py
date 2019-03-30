from intelhex import IntelHex
import sys
from bootclass import MyBoot
import math

port = sys.argv[1]
speed = int(sys.argv[2])
file = sys.argv[3]

hex = IntelHex(file)
data = list(hex.tobinarray())
size = len(data)

print(port)
print(speed)
print(file)
print(size)

iterations = math.ceil(size / 128)

boot = MyBoot(port, speed)
boot.open()

for i in range(0, iterations):
    addr = i * 128
    boot.setAddr(addr)
    if  i == (iterations - 1):
        data0 = data[addr:]
        boot.writeData(data0)
    else:
        data0 = data[addr:(addr + 128)]
        boot.writeData(data0)

boot.close()
