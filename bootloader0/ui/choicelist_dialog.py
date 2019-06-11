from tkinter import *
def choicebox(choicelist, altnames):
    choicewin = Tk()
    def buttonfn(): 
        choicewin.destroy()
    choicewin.resizable(False, False)
    choicewin.title("ChoiceBox")

    Label(choicewin, text="Select an item:").grid(row=0, column=0, sticky="W")

    var = StringVar(choicewin)
    var.set(choicelist[0]) # default option
    popupMenu = OptionMenu(choicewin, var, *choicelist)
    popupMenu.grid(sticky=N+S+E+W, row =1, column = 0)

    Button(choicewin, text='Done', command=buttonfn).grid(row=2, column=0)
    choicewin.mainloop()
    v = var.get()
    for i in range(len(choicelist)):
        if v == choicelist[i]:
            return altnames[i]

if __name__ == '__main__':
    print(choicebox(['attiny2313', 'atmega328p'],['t2313','m328p']))
