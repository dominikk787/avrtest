from intelhex import IntelHex
import sys
from bootclass import MyBoot
import math

def read(boot, file, size, linesize = 32):
    iterations = round(size / 128)

    hex = IntelHex()

    data = []

    for i in range(0, iterations):
        start = i * 128
        boot.setAddr(start)
        datan = list(boot.readData(128))
        data = data + datan
        
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

    hex.write_hex_file(file, byte_count=linesize)
    return hex

def exit(boot):
    boot.exitBoot()

def write(boot, file):
    hex = IntelHex(file)
    data = list(hex.tobinarray())
    size = len(data)

    iterations = math.ceil(size / 128)

    for i in range(0, iterations):
        addr = i * 128
        boot.setAddr(addr)
        if  i == (iterations - 1):
            data0 = data[addr:]
            boot.writeData(data0)
        else:
            data0 = data[addr:(addr + 128)]
            boot.writeData(data0)

def version(boot):
    v = boot.getVersion()
    v0 = round(v / 256)
    v1 = v % 256
    v = [v0, v1]
    return v

def signature(boot):
    sig = boot.getSignature()
    sig = hex(sig)
    return sig

if __name__ == '__main__':
    port = input('Port: ')
    baud = input('Baudrate: ')
    boot = MyBoot(port, baud)
    boot.open()
    command = ' '
    while command[0] != 'X':
        command = input('Command: ')
        args = command.split()
        print(args)
        if command[0] == 'R':
            print('R')
            if len(args) > 3:
                read(boot, args[1], int(args[2], base=16), int(args[3]))
            else:
                read(boot, args[1], int(args[2], base=16))
        elif command[0] == 'Q':
            print('Q')
            exit(boot)
        elif command[0] == 'P':
            print('P')
            write(boot, args[1])
        elif command[0] == 'V':
            print('V')
            print(version(boot))
        elif command[0] == 'S':
            print('S')
            print(signature(boot))
    boot.close()
