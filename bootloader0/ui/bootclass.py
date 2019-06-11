import serial
import sys
import math
from intelhex import IntelHex
from time import sleep

class MyBoot:
    def __init__(self, port, baudrate):
        self.serial = serial.Serial()
        self.serial.port = port
        self.serial.baudrate = baudrate
        self.serial.rts = False
    def open(self):
        self.serial.open()
    def close(self):
        self.serial.close()
    def start2313Boot(self):
        self.serial.flush()
        self.serial.reset_input_buffer()
        self.serial.rts = True
        sleep(1)
        self.serial.rts = False
    def writecommand(self, command, waitfordata = True):
        if self.serial.is_open:
            self.serial.flush()
            self.serial.reset_input_buffer()
            self.serial.write(bytes(command))
            if not waitfordata:
                return b''
            while self.serial.read() != b';':
                i = 0
            length = self.serial.read()[0]
            print(length)
            if length == 0:
                while self.serial.read() != b':':
                    i = 0
                return b''
            data = self.serial.read(length)
            print(data)
            while self.serial.read() != b':':
                i = 0
            return data
    def setAddr(self, addrB):
        addr = round(addrB / 2)
        addrL = addr % 256
        addrH = round((addr - addrL)/256)
        self.writecommand((b'A' + bytes([addrL, addrH, 32])))
    def set2313Addr(self, addrP):
        addrP %= 64
        addr = self.writecommand((B'A' + bytes([addrP, 32])))
        addrb = addr[0] * 256
        addrb += addr[1]
        return addrb
    def getAddr(self):
        addr = self.writecommand(b'a ')
        addrL = addr[0]
        addrH = addr[1]
        addrB = addrL + (addrH * 256)
        return addrB
    def getVersion(self):
        data0 = self.writecommand(b'G1 ')
        data1 = self.writecommand(b'G0 ')
        data0 = data0[0]
        data1 = data1[0]
        version = [data1, data0]
        return version
    def getSignature(self):
        data = self.writecommand(b'S ')
        data0 = data[0]
        data1 = data[1]
        data2 = data[2]
        sig = (data0 * 65536) + (data1 * 256) + data2
        return hex(sig)
    def exitBoot(self):
        self.writecommand(b'Q.', waitfordata=False)
    def readData(self, size):
        size = size % 256
        data = self.writecommand(b'R' + bytes([0, size, 32]))
        return data
    def read2313Data(self, size):
        size = size % 65
        data = self.writecommand(b'R' + bytes([size, 32]))
        return data
    def writeData(self, data):
        if type(data) != type([]):
            raise TypeError('data must be a list')
        size = len(data)
        if size > 255:
            raise ValueError('Too many elements to write')
        data0 = list(b'P') + [0, size] + data + [32]
        print(data0)
        self.writecommand(bytes(data0))
    def write2313Data(self, data):
        if type(data) != type([]):
            raise TypeError('data must be a list')
        size = len(data)
        if size > 64:
            raise ValueError('Too many elements to write')
        data0 = list(b'P') + [size] + data + [32]
        #print(data0)
        self.writecommand(bytes(data0))
    def readFlash(self, file, size, linesize = 32):
        iterations = round(size / 128)

        hex = IntelHex()

        data = []

        for i in range(0, iterations):
            start = i * 128
            self.setAddr(start)
            datan = list(self.readData(128))
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
    def read2313Flash(self, file, size, linesize = 32):
        iterations = round(size / 32)

        hex = IntelHex()

        data = []

        for i in range(0, iterations):
            print(self.set2313Addr(i))
            datan = list(self.read2313Data(32))
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

    def writeFlash(self, file):
        hex = IntelHex(file)
        data = list(hex.tobinarray())
        size = len(data)

        iterations = math.ceil(size / 128)

        for i in range(0, iterations):
            addr = i * 128
            self.setAddr(addr)
            if  i == (iterations - 1):
                data0 = data[addr:]
                self.writeData(data0)
            else:
                data0 = data[addr:(addr + 128)]
                self.writeData(data0)
    def write2313Flash(self, file):
        hex = IntelHex(file)
        data = list(hex.tobinarray())
        size = len(data)

        iterations = math.ceil(size / 32)

        for i in range(0, iterations):
            addr = i * 32
            print(self.set2313Addr(i))
            if  i == (iterations - 1):
                data0 = data[addr:]
                self.write2313Data(data0)
            else:
                data0 = data[addr:(addr + 32)]
                self.write2313Data(data0)

if __name__ == '__main__':
    boot = MyBoot('com14', 38400)
    boot.open()
    boot.start2313Boot()
    #boot.read2313Flash('readflash.hex', 2048, linesize = 16)
    #print(boot.getAddr())
    #print(boot.getVersion())
    #print(hex(boot.getSignature()))
    boot.close()
