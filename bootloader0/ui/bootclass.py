import serial
import sys

class MyBoot:
    def __init__(self, port, speed):
        self.serial = serial.Serial()
        self.serial.port = port
        self.serial.baudrate = speed
    def open(self):
        self.serial.open()
    def close(self):
        self.serial.close()
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
            if length == 0:
                return b''
            data = self.serial.read(length)
            while self.serial.read() != b':':
                i = 0
            return data
    def setAddr(self, addrB):
        addr = round(addrB / 2)
        addrL = addr % 256
        addrH = round((addr - addrL)/256)
        self.writecommand((b'A' + bytes([addrL, addrH, 32])))
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
        version = (data1 * 256) + data0
        return version
    def getSignature(self):
        data = self.writecommand(b'S ')
        data0 = data[0]
        data1 = data[1]
        data2 = data[2]
        sig = (data0 * 65536) + (data1 * 256) + data2
        return sig
    def exitBoot(self):
        self.writecommand(b'Q.', waitfordata=False)
    def readData(self, size):
        size = size % 256
        data = self.writecommand(b'R' + bytes([0, size, 32]))
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

if __name__ == '__main__':
    boot = MyBoot('com3', 78600)
    boot.open()
    boot.setAddr(int(sys.argv[1], base=16))
    print(boot.getAddr())
    print(boot.getVersion())
    print(hex(boot.getSignature()))
    print(list(boot.readData(255)))
    boot.close()
