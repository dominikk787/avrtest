import serial

ser = serial.Serial('com14', 9600)

while True:
    n = int(input())
    if n > 255 or n < 0:
        break
    d = bytes([n])
    ser.write(d)
ser.close()
