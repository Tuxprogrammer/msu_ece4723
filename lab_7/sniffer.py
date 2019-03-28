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

import serial
import keyboard

import time
from threading import Thread
from queue import Queue
import sys
from glob import glob

baud = 230400


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
    

def choose_mask_num(event):
    """ Ask the user for choice in mask, not entirely sure what mask format is"""
    mask_choice = int(input("\nChoose mask num:\n"))
    
    return mask_choice


def data_handler(q, mask):
    # TODO: get message from queue, filter based on incoming message type, and user choice of filter, print
    # switch on mask....
    #q.get() then ->
    #q.task_done()
    return


def read_from_port(q, ser):
    while True:
        bytes_in = ser.inWaiting()
        q.put(ser.read(bytes_in))


def main():
    """Main terminal app."""
    ports = gather_ports()
    port = choose_pic_port(ports);

    # connect chosen port
    ser = serial.Serial(ports[port])
    
    # serial data queue
    q = Queue(maxsize=0)
    
    # set up thread for receiving data
    reader = Thread(target=read_from_port, args=(q, ser))
    reader.start()

    # set up keyboard interrupt for mask choice
    print("Remember: Press SHIFT key to reset mask...")
    keyboard.on_press_key(42,choose_mask_num)

    mask = None
    while True:
        time.sleep(1)
        data_handler(q, mask)
        

if __name__ == "__main__":
    main()