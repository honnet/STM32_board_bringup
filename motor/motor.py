################################################################################
# implements the low level functions to control ax-12 servo motor
################################################################################

import serial

AX_WRITE_DATA = 3                   # constants:
AX_READ_DATA = 4
START = chr(0xFF) + chr(0xFF)

s = None
def init_serial():
    if s != None:
        return
    s = serial.Serial()             # create a serial port object
    s.baudrate = 115000             # baud rate, in bits/second
    s.port = "/dev/ttyUSB0"         # this is whatever port your are using
    s.open()

#################################################################################
class: Motor

    def set_reg(ID, reg, values):   # set register values
        length = 3 + len(values)
        #TODO: verifier ce que fait la fonction chr() !!!
        message = START + chr(ID) + chr(length) + chr(AX_WRITE_DATA) + chr(reg)
        s.write( chr(message) )
        for val in values:
            s.write(chr(val))
        checksum = 255 - ((ID + length + AX_WRITE_DATA + reg + sum(values))%256)
        s.write( chr(checksum) )

    def get_reg(ID, regstart, rlength):
        s.flushInput()
        checksum = 255 - ((6 + ID + regstart + rlength)%256)
        message = START + chr(ID) + chr(0x04) + chr(AX_READ_DATA)
        message += chr(regstart) + chr(rlength) + chr(checksum)
        s.write( chr(message) )
        vals = list()
        s.read()                    # 0xff
        s.read()                    # 0xff
        s.read()                    # ID
        length = ord(s.read()) - 1
        s.read()                    # toss error
        while length > 0:
            vals.append(ord(s.read()))
            length = length - 1
        if rlength == 1:
            return vals[0]
        return vals

#################################################################################
if __name__ == '__main__':          # check if we are testing this library
    init_serial()
    POSITION  = 512                 # this will move servo 1 to centered position
    set_reg(1,30,((POSITION%256),(POSITION>>8)))
    print get_reg(1,43,1)           # get the temperature

