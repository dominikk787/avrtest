import wx
import serial
import threading

class Example(wx.Frame):

    def __init__(self, parent, title):
        super(Example, self).__init__(parent, title=title)

        self.serial = -1

        self.InitUI()
        self.Centre()

    def InitUI(self):

        panel = wx.Panel(self)

        font = wx.SystemSettings.GetFont(wx.SYS_SYSTEM_FONT)

        font.SetPointSize(9)

        vbox = wx.BoxSizer(wx.VERTICAL)

        hbox1 = wx.BoxSizer(wx.HORIZONTAL)
        st1 = wx.StaticText(panel, label='Port')
        st1.SetFont(font)
        hbox1.Add(st1, flag=wx.RIGHT, border=8)
        self.port = wx.TextCtrl(panel)
        hbox1.Add(self.port, proportion=1)
        st2 = wx.StaticText(panel, label=' Speed')
        st2.SetFont(font)
        hbox1.Add(st2, flag=wx.RIGHT, border=8)
        self.speed = wx.TextCtrl(panel)
        hbox1.Add(self.speed, proportion=1)
        hbox1.Add((10,0))
        self.btn1 = wx.Button(panel, label='Connect', size=(80, 30))
        self.btn1.Bind(wx.EVT_BUTTON, self.onButtonConnectClick)
        hbox1.Add(self.btn1)
        vbox.Add(hbox1, flag=wx.EXPAND|wx.LEFT|wx.RIGHT|wx.TOP, border=10)

        vbox.Add((-1, 1))

        hbox2 = wx.BoxSizer(wx.HORIZONTAL)
        st2 = wx.StaticText(panel, label='Data to send')
        hbox2.Add(st2, flag=wx.RIGHT, border=8)
        self.tc3 = wx.TextCtrl(panel, style=wx.TE_PROCESS_ENTER)
        self.tc3.Bind(wx.EVT_TEXT_ENTER, self.onEnterDataSend)
        hbox2.Add(self.tc3, proportion=1)
        vbox.Add(hbox2, flag=wx.EXPAND|wx.LEFT|wx.RIGHT|wx.TOP, border=10)

        hbox2 = wx.BoxSizer(wx.HORIZONTAL)
        st3 = wx.StaticText(panel, label='Received data')
        hbox2.Add(st3, flag=wx.RIGHT, border=8)
        vbox.Add(hbox2, flag=wx.EXPAND|wx.LEFT|wx.RIGHT|wx.TOP, border=10)

        vbox.Add((-1, 5))

        hbox3 = wx.BoxSizer(wx.HORIZONTAL)
        self.tc2 = wx.TextCtrl(panel, style=wx.TE_MULTILINE | wx.TE_READONLY)
        hbox3.Add(self.tc2, proportion=1, flag=wx.EXPAND)
        vbox.Add(hbox3, proportion=1, flag=wx.LEFT|wx.RIGHT|wx.EXPAND,
            border=10)

        vbox.Add((-1, 10))
        
        panel.SetSizer(vbox)

    def onButtonConnectClick(self, e):
        if self.serial == -1:
            self.serial = serial.Serial(self.port.GetValue(),int(self.speed.GetValue()),timeout=0)
            print(self.serial)
            self.btn1.SetLabel('Disconnect')
            self.serialthread = serialthread(1, self.serial, self.tc2)
            self.serialthread.start()
        else:
            self.serialthread.exit = True
            self.serial.close()
            print(self.serial)
            self.serial = -1
            self.btn1.SetLabel('Connect')

    def onEnterDataSend(self, e):
        if self.serial != -1:
            b = bytes(self.tc3.GetValue(), 'utf-8')
            self.tc2.SetValue(self.tc2.GetValue() + '\r\n')
            i = 0
            while i < len(b):
                if (chr(b[i]) == '0') and (len(b) > (i + 3)):
                    if chr(b[i+1]) == 'x':
                        ch0 = chr(b[i+2])
                        ch1 = chr(b[i+3])
                        hexc = '' + ch0 + ch1
                        d = int(hexc, base=16)
                        print(d)
                        b0 = 0
                        if i == 0:
                            b0 = bytes()
                        else:
                            b0 = b[0:i]
                        b0 = b0 + bytes([d])
                        if len(b) > (i + 5):
                            b0 = b0 + b[(i+4):(len(b))]
                        print(b0)
                        b = b0
                i = i + 1
            self.serial.write(b)
            self.tc3.SetValue('')

class serialthread(threading.Thread):
    def __init__(self, threadID, serial, outputtext):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.serial = serial
        self.exit = False
        self.outputtext = outputtext
    def run(self):
        while not self.exit:
            if self.serial.in_waiting > 0:
                ch = self.serial.read()
                self.outputtext.SetValue(self.outputtext.GetValue() + hex(ord(ch)) + ' ') 

def main():

    app = wx.App()
    ex = Example(None, title='MyBoot control')
    ex.Show()
    app.MainLoop()


if __name__ == '__main__':
    main()
