import argparse

def parseargs():
    parser = argparse.ArgumentParser(description='Bootloader control for MyBoot (atmega328p, attiny2313).')
    parser.add_argument('-p', '--proc', action = 'store', dest = 'proc', default = None)
    parser.add_argument('-f', '--file', action = 'store', dest = 'file', default = '')
    args = parser.parse_args()
    proc = args.proc
    filename = args.file
    return proc, filename
    
parseargs()
