from tkinter import *
from tkinter import filedialog
from bootclass import MyBoot
from choicelist_dialog import choicebox
from argsparser import parseargs
from preferences import readpreferences, writepreferences
import os

boot = -1
procsnames = ['Tiny2313', 'Mega328p']
procs = ['t2313', 'm328p']
proc = None
filename = ''
proc, filename = parseargs()
if proc == None:
    proc = choicebox(procsnames, procs)

def connect():
    global boot
    print(boot)
    if boot == -1:
        baudrate = int(baud.get())
        portname = port.get()
        boot = MyBoot(portname, baudrate)
        boot.open()
        conb.config(text = 'Disconnect')
        writepreferences({'baudrate': baudrate, 'port': portname})
    else:
        boot.close()
        conb.config(text = 'Connect')
        boot = -1

def fileopen():
    dir = readpreferences().get('directory',os.getcwd())
    filename = -1
    if command.get() == 1:
        filename = filedialog.askopenfilename(initialdir=dir, filetypes=[['Hex files (*.hex)','*.hex']])
    elif command.get() == 2:
        filename = filedialog.asksaveasfilename(initialdir=dir,filetypes=[['Hex files (*.hex)','*.hex']])
    if filename != -1:
        filepath = os.path.dirname(filename)
        writepreferences({'directory': filepath})
        file.delete(0, END)
        file.insert(0, filename)

def go():
    if boot != -1:
        if proc == 't2313':
            if command.get() == 1:
                boot.write2313Flash(file.get())
                text.insert(END, 'File writed\r\n')
            elif command.get() == 2:
                boot.read2313Flash(file.get(), 2048)
                text.insert(END, 'File readed\r\n')
            elif command.get() == 3:
                boot.start2313Boot()
                text.insert(END, 'Bootloader started\r\n')
            elif command.get() == 4:
                boot.exitBoot()
                text.insert(END, 'Bootloader exited\r\n')
        elif proc == 'm328p':
            if command.get() == 1:
                boot.writeFlash(file.get())
                text.insert(END, 'File writed\r\n')
            elif command.get() == 2:
                boot.readFlash(file.get(), 2048)
                text.insert(END, 'File readed\r\n')
            elif command.get() == 3:
                sig = boot.getSignature()
                text.insert(END, 'Signature: ' + sig + '\r\n')
            elif command.get() == 4:
                boot.exitBoot()
                text.insert(END, 'Bootloader exited\r\n')

preferences = readpreferences()
portname = preferences.get('port','')
baudrate = preferences.get('baudrate','76800')

top = Tk()
frameconnect = Frame(top, bd = 5)
frameconnect.pack(side = TOP, fill = X)
L1 = Label(frameconnect, text="Port: ")
L1.pack( side = LEFT)
port = Entry(frameconnect, width = 6)
port.delete(0,END)
port.insert(0, portname)
port.pack( side = LEFT, fill = X, expand = True)
L2 = Label(frameconnect, text=" Baudrate: ")
L2.pack( side = LEFT)
baud = Spinbox(frameconnect, width = 6, values = [38400, 76800, 115200])
baud.delete(0,END)
baud.insert(0,str(baudrate))
baud.pack( side = LEFT, fill = X, expand = True)
L = Label(frameconnect)
L.pack( side = LEFT)
conb = Button(frameconnect, text="Connect", command=connect)
conb.pack( side = LEFT)

frameoperation = Frame(top, bd = 5)
frameoperation.pack( side = TOP, fill = X)
L3 = Label(frameoperation, text="File: ")
L3.pack( side = LEFT)
file = Entry(frameoperation, width = 40)
file.pack( side = LEFT, fill = X, expand = True)
file.delete(0, END)
file.insert(0, filename)
L = Label(frameoperation)
L.pack( side = LEFT)
fileb = Button(frameoperation, text="Select", command=fileopen)
fileb.pack( side = LEFT)

frameoperation1 = Frame(top, bd = 5)
frameoperation1.pack( side = TOP, fill = X)
command = IntVar()
R1 = Radiobutton(frameoperation1, text="Program Flash", variable=command, value=1)
R2 = Radiobutton(frameoperation1, text="Read Flash", variable=command, value=2)
R3 = 0
if proc == 'm328p':
    R3 = Radiobutton(frameoperation1, text="Read Signature", variable=command, value=3)
elif proc == 't2313':
    R3 = Radiobutton(frameoperation1, text="Start Bootloader", variable=command, value=3)
R4 = Radiobutton(frameoperation1, text="Exit Bootloader", variable=command, value=4)
R1.pack(side = LEFT, fill = X, expand = True)
R2.pack(side = LEFT, fill = X, expand = True)
R3.pack(side = LEFT, fill = X, expand = True)
R4.pack(side = LEFT, fill = X, expand = True)
gob = Button(frameoperation1, text="Go", command=go)
gob.pack( side = LEFT, expand = True)

frameoutput = Frame(top, bd = 5)
frameoutput.pack( side = TOP, fill = BOTH, expand = True)
frametext = Frame(frameoutput)
scrollbar = Scrollbar(frametext)
text = Text(frametext, yscrollcommand=scrollbar.set)
text.bind("<Key>", lambda e: "break")
scrollbar.config(command=text.yview)
scrollbar.pack(side=RIGHT, fill=Y)
text.pack(side=LEFT, fill=BOTH, expand=True)
frametext.pack(fill=BOTH, expand = True)
top.mainloop()
