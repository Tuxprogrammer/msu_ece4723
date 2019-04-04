sniffer.py User Guide

Dependencies:
- Python 3 and stdlib
- Pyserial (pip install)
- Keyboard (pip install)

Instructions:

1. Build and program the sniffing board with t7sniff.c
2. Run the following command from a terminal:

```bash
python [path_to]/sniffer.py
```

3.	Pick the port the t7sniff.c is connected to.
4.	Watch the serial messages come in.
5.	If a filter is wanted, hit Ctrl+F to enter a new mask in the form “0x000”
6.	If a filter is not wanted, hit Ctrl+F and enter nothing to remove mask
