import serial
import threading
ser = serial.Serial('COM14',38400,timeout=0)
print(ser.name)
exitFlag = 0
sendData = -1

class readInputThread (threading.Thread):
   def __init__(self, threadID, name):
      threading.Thread.__init__(self)
      self.threadID = threadID
      self.name = name
      self.data = -2
   def run(self):
      self.data = readInputFunction(self.name)

def readInputFunction(threadName):
    ch = input()
    if ch != 'x':
       return bytes([int(ch, base = 16)])
    else:
        return -1

thread0 = readInputThread(0,"thread0")
thread0.daemon = True
thread0.start()
        
while True:
    b = ser.read()
    try:
        d = list(b)[0]
        print(hex(d))
    except:
        d = 0

    if thread0.data != -2:
        if thread0.data != -1:
            ser.write(thread0.data)
            thread0 = readInputThread(0,"thread0")
            thread0.daemon = True
            thread0.start()
        else:
            break
ser.close()
