"""
/* Embedded Systems - Spring 2019

 * Christian Bush (cbb330@msstate.edu), Spencer Callicott (sc2257@msstate.edu)
 * Will Carroll (woc17@msstate.edu), Landon Casey (lec426@msstate.edu)
 * Jack Fletcher (jdf469@msstate.edu)
 *
 * sniffer.py - ran on PC to sniff and filter CAN messages from the pic
 * 
 * Dependencies(pip install $): pyserial, keyboard
 * 
 * If no options for ports: check that python has permission to com ports
 */
"""

# https://keyboardinterrupt.org/catching-a-keyboardinterrupt-signal/

import serial
import keyboard

import time
from threading import Thread
from queue import Queue
import sys
from glob import glob


class CanMsg:
    """
    Case Class to contain a single Can Message and relevant data
    # Types
    #   define CANMSG_TYPE_POLL 0
    #   define CANMSG_TYPE_BEACON 1
    #   define CANMSG_TYPE_TEMPERATURE1 2
    #   define CANMSG_TYPE_TEMPERATURE2 3
    #   define CANMSG_TYPE_WAVEFORM 4
    #   define CANMSG_TYPE_POTENTIOMETER 5
    #   define CANMSG_TYPE_FREQUENCY 6
    #   define CANMSG_TYPE_AMPLITUDE 7
    #   define CANMSG_TYPE_DUTYCYCLE 8
    #   define CANMSG_TYPE_LEDS 9
    # ID
    # Data
    """

    def __init__(self, type=0, id=0):
        self.type = type
        self.id = id
        self.data = []

    def getType(self):
        return self.type

    def getId(self):
        return self.id

    def getData(self):
        return self.data

    def setType(self, type):
        self.type = type

    def setId(self, id):
        self.id = id

    def clearData(self):
        self.data.clear()

    def appendData(self, data):
        self.data.append(data)

    def __str__(self):
        if len(self.data) != 0:
            return "Msg Type: %d\n\tCan ID: %s\n\tBytes recvd:\n%s" % (self.type, '0x{0:08X}'.format(self.id), str(self.data))
        else:
            return "Msg Type: %d\n\tCan ID: %s\n" % (self.type, '0x{0:08X}'.format(self.id))


baud = 230400
port = "/dev/ttyUSB0"
output_state = True
mask_choice = None


def gather_ports():
    """Gather all serial ports found on system."""
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        ports = glob('/dev/ttyUSB*')
    elif sys.platform.startswith('darwin'):
        ports = glob('/dev/ttyUSB.*')
    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    for port in ports:
        try:
            ser = serial.Serial(port)
            ser.close()
            result.append(port)
        except (OSError, serial.SerialException):
            print(serial.SerialException)
            pass
    return result


def choose_pic_port(ports):
    """ Ask the user for choice in port """
    for x in range(len(ports)):
        print(x, '. ', ports[x], sep='')

    port_choice = int(input("Choose the pic port:\n"))

    if port_choice > len(ports):
        print("Choice out of bounds.")
        exit()

    return port_choice


def choose_mask_num():
    global output_state, mask_choice
    """ Ask the user for choice in mask, not entirely sure what mask format is"""
    output_state = False
    while True:
        mask_input = input("\nChoose mask num:\n")
        if mask_input == '':
            mask_choice = None
            break
        else:
            try:
                mask_choice = int(mask_input, 0)
                break
            except:
                mask_choice = None
    output_state = True


def data_handler(q):
    global output_state, mask_choice

    canmsg = q.get()
    if output_state is True:
        if mask_choice is not None:
            if mask_choice == canmsg.getId():
                print(str(canmsg), end='\n')
            else:
                print('Doesnt match mask')
        else:
            print(str(canmsg), end='\n')


def read_from_port(q, ser):
    canmsg = CanMsg()

    # Example Message
    # Msg Type: 0
    #   Can ID: 0x000007A0
    #   Bytes recvd:
    #   0x00: 0x00
    #   0x01: 0x00
    # END
    while True:
        newline = ser.readline()
        try:
            newline = newline.decode('utf-8')
        except:
            continue

        if 'Msg Type: ' in newline:
            canmsg.clearData()
            msgtype = 0
            try:
                msgtype = int(newline.split(':')[1].strip(), 10)
            except:
                pass
            canmsg.setType(msgtype)
        elif '\tCan ID: ' in newline:
            canid = 0x00
            try:
                canid = int(newline.split(':')[1].strip(), 0)
                canmsg.setId(canid)
            except:
                pass
        elif '\tBytes recvd:' in newline:
            pass
        elif '\t' in newline:
            data = 0
            try:
                data = int(newline.split(':')[1].strip(), 0)
                canmsg.appendData(data)
            except:
                pass

        elif 'END' in newline:
            # This HAS to do a deep copy of canmsg
            q.put(canmsg)


def main():
    """Main terminal app."""

    ports = gather_ports()
    port = choose_pic_port(ports)

    # connect chosen port
    ser = serial.Serial(ports[port], baud, timeout=2)
    ser.setRTS(1)
    time.sleep(0.01)  # sleep 1 ms
    ser.setRTS(0)

    # serial data queue
    q = Queue(maxsize=0)

    # set up thread for receiving data
    reader = Thread(target=read_from_port, args=(q, ser))
    reader.start()

    # set up keyboard interrupt for mask choice
    print("Remember: Press CTRL+F key to reset mask...")
    keyboard.add_hotkey('ctrl+f', choose_mask_num)

    while True:
        time.sleep(50.0/1000)
        data_handler(q)


if __name__ == "__main__":
    main()
