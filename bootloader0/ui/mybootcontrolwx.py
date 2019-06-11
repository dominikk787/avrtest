import wx
from bootclass import MyBoot

class Example(wx.Frame):

    def __init__(self, parent, title):
        super(Example, self).__init__(parent, title=title)

        self.boot = -1

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
        self.btn1 = wx.Button(panel, label='Connect', size=(80, 25))
        self.btn1.Bind(wx.EVT_BUTTON, self.onButtonConnectClick)
        hbox1.Add(self.btn1)
        vbox.Add(hbox1, flag=wx.EXPAND|wx.LEFT|wx.RIGHT|wx.TOP, border=10)

        vbox.Add((-1, 1))

        sb0 = wx.StaticBox(panel)

        sboxs = wx.StaticBoxSizer(sb0, wx.VERTICAL)
        hbox2 = wx.BoxSizer(wx.HORIZONTAL)
        st2 = wx.StaticText(panel, label='File')
        hbox2.Add(st2, flag=wx.RIGHT, border=8)
        self.tc3 = wx.TextCtrl(panel)
        hbox2.Add(self.tc3, proportion=1)
        self.btn2 = wx.Button(panel, label='File', size=(40, 24))
        self.btn2.Bind(wx.EVT_BUTTON, self.onButtonFileClick)
        hbox2.Add(self.btn2)
        sboxs.Add(hbox2, flag=wx.EXPAND|wx.LEFT|wx.RIGHT|wx.TOP, border=5)

        hbox3 = wx.BoxSizer(wx.HORIZONTAL)
        self.rb1 = wx.RadioButton(panel, label='Program', style=wx.RB_GROUP)
        hbox3.Add(self.rb1, flag=wx.RIGHT, border=8)
        self.rb2 = wx.RadioButton(panel, label='Read')
        hbox3.Add(self.rb2, flag=wx.RIGHT, border=8)
        self.rb3 = wx.RadioButton(panel, label='Signature')
        hbox3.Add(self.rb3, flag=wx.RIGHT, border=8)
        self.rb4 = wx.RadioButton(panel, label='Reset')
        hbox3.Add(self.rb4, flag=wx.RIGHT, border=8)
        self.btn3 = wx.Button(panel, label='Go', size=(40, 20))
        self.btn3.Bind(wx.EVT_BUTTON, self.onButtonGoClick)
        hbox3.Add(self.btn3)
        sboxs.Add(hbox3, flag=wx.EXPAND|wx.LEFT|wx.RIGHT|wx.TOP, border=5)
        vbox.Add(sboxs, flag=wx.EXPAND|wx.LEFT|wx.RIGHT|wx.TOP, border=5)

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
        if self.boot == -1:
            self.btn1.SetLabel('Disconnect')
            self.boot = MyBoot(self.port.GetValue(), int(self.speed.GetValue()))
            self.boot.open()
        else:
            self.boot.close()
            self.boot = -1
            self.btn1.SetLabel('Connect')

    def onButtonFileClick(self, e):
        if self.rb3.GetValue() or self.rb4.GetValue():
            return
        style = 0
        if self.rb1.GetValue():
            style = wx.FD_FILE_MUST_EXIST | wx.FD_OPEN
        else:
            style = wx.FD_SAVE | wx.FD_OVERWRITE_PROMPT
        fileDialog = wx.FileDialog(self, "Open HEX file", wildcard="HEX files (*.hex)|*.hex", style=style)
        if fileDialog.ShowModal() == wx.ID_CANCEL:
            return     # the user changed their mind

        # Proceed loading the file chosen by the user
        pathname = fileDialog.GetPath()
        print(pathname)
        self.tc3.SetValue(pathname)

    def onButtonGoClick(self, e):
        if self.boot != -1:
            if self.rb1.GetValue():
                self.boot.writeFlash(self.tc3.GetValue())
                self.tc2.SetValue(self.tc2.GetValue() + "Writing ended\r\n")
            elif self.rb2.GetValue():
                self.boot.readFlash(self.tc3.GetValue(), 32256)
                self.tc2.SetValue(self.tc2.GetValue() + "Reading ended\r\n")
            elif self.rb3.GetValue():
                sig = self.boot.getSignature()
                self.tc2.SetValue(self.tc2.GetValue() + "Signature: " + sig + "\r\n")
            elif self.rb4.GetValue():
                self.boot.exitBoot()
                self.tc2.SetValue(self.tc2.GetValue() + "Bootloader exited\r\n")

def main():

    app = wx.App()
    ex = Example(None, title='MyBoot control')
    ex.Show()
    app.MainLoop()


if __name__ == '__main__':
    main()
