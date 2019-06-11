import json

def readpreferences():
    filename = r'C:\Users\programowanie\Documents\Atmel Studio\7.0\avrtest\bootloader0\ui\preferences.json'
    file = open(filename, 'r')
    pref = json.loads(file.readline())
    file.close()
    return pref

def writepreferences(newpreferences):
    preferences = readpreferences()
    preferences.update(newpreferences)
    filename = r'C:\Users\programowanie\Documents\Atmel Studio\7.0\avrtest\bootloader0\ui\preferences.json'
    file = open(filename, 'w')
    file.write(json.dumps(preferences))
    file.close()
