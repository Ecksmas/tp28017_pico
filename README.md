# TP28017 for the pico

This repo aims to establishing communication via a Raspberry Pi Pico 2 and a 240x320 Elegoo screen with a TP28017 controller and ILI9341 chipset.


## Commands to connect serial monitor of pico2 in WSL

### Powershell(Admin) commands

The port of the unit might differ, mine is 5-1

1. usbipd list
2. If you have not bound the usb port: usbipd bind --busid 5-1
3. usbipd attach --wsl --busid 5-1

### WSL commands

ACM0 may differ from your unit

1. sudo minicom -D /dev/ttyACM0 -b 115200
