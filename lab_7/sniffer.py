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

from collections import namedtuple

CanMsg = namedtuple("CanMsg", "type canid data")

baud = 230400
port = "/dev/ttyUSB0"
output_state = True
mask_choice = None


def gather_ports():
    """Gather all serial ports found on system."""
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        ports = glob('/dev/tty[A-Za-z]*')
    elif sys.platform.startswith('darwin'):
        ports = glob('/dev/tty.*')
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
    mask_input = input("\nChoose mask num:\n")
    if mask_input == '':
        mask_choice = None
    else:
        mask_choice = int(mask_input, 16)
    output_state = True


def data_handler(q):
    global output_state, mask_choice
    # TODO: get message from queue, filter based on incoming message type, and user choice of filter, print
    # switch on mask....
    msg = q.get()
    if output_state is True:
        try:
            msg = msg.decode('utf-8')
            if mask_choice is not None:
                if '0x{0:08X}'.format(mask_choice) in msg:
                    print(msg, end='\n')
                else:
                    print('Doesnt match mask')
            else:
                print(msg, end='\n')
        except:
            pass


def read_from_port(q, ser):
    while True:
        msg = ser.readline()
        while b'Msg Type:' not in msg:
            msg = ser.readline()

        #
        # four lines for standard message

        q.put(msg)


def main():
    """Main terminal app."""

    # connect chosen port
    ser = serial.Serial(port, baud, timeout=2)
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
    if len(sys.argv) > 1:
        port = sys.argv[2]
    main()
