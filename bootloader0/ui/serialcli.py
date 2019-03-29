import serial
import threading
import sys

class serialthread(threading.Thread):
    def __init__(self, threadID, serial):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.serial = serial
        self.exit = False
    def run(self):
        while not self.exit:
            ch = self.serial.read()
            if len(ch) == 0:
                self.exit = True
                break
            h = format(ord(ch), 'X')
            if len(h) == 1:
                h = '0' + h
            else:
                h = '' + h
            print(h, end='')

serial = serial.Serial(sys.argv[1],int(sys.argv[2]),timeout=0.1)

dts = sys.argv[3]
thread0 = serialthread(1, serial)
thread0.start()
b = bytes(dts, 'utf-8')
i = 0
while i < len(b):
    if (chr(b[i]) == '0') and (len(b) > (i + 3)):
        if chr(b[i+1]) == 'x':
            ch0 = chr(b[i+2])
            ch1 = chr(b[i+3])
            hexc = '' + ch0 + ch1
            d = int(hexc, base=16)
            b0 = 0
            if i == 0:
                b0 = bytes()
            else:
                b0 = b[0:i]
            b0 = b0 + bytes([d])
            if len(b) > (i + 4):
                b0 = b0 + b[(i+4):]
            b = b0
    i = i + 1
#print(b)
serial.write(b)
while not thread0.exit:
    i = i
serial.close()
