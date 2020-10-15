***************
* Xany2Sounds *
***************

Xany2Sounds is a sound module that use the X-Any feature for command un JQ6500-16p or JQ6500-28p.
I use a Pro Micro board.

For use it, you need:
- An OpenAVRc Transmitter with the X-Any feature:
- A PCF8574 board.
- Height switch.

See our documentation here for all connections:
https://github.com/Ingwie/OpenAVRc_Dev/blob/V3/documentation/Compilez%20votre%20FW%20OpenAVRc_V3.pdf

Each switch command one sound.
With 8 switchs you can command 8 sounds.
But the JQ6500 has 5 input that can be used for command 5 sounds.

So, it's possible to command 13 sounds.
The JQ6500 inputs commands sounds from 1 to 5 and X-Any switchs command sounds from 6 to 13.
You can select this mode or not (shift mode) when your are in the configuration mode.
Exemple as use, connect the K1 to K5 inputs to 5 relay and you can use the JQ6500 as alarm in a boat .
K1 to K5 are active with a ground connection.

The module accept several input mode.
PWM, CPPM, SBUS, SRXL, SUMD, IBUS or JETI.
You can select also these modes in confuration menu.

How to compile:
Add these Sparkfun boards in the Arduino IDE preferences.
https://raw.githubusercontent.com/sparkfun/Arduino_Boards/master/IDE_Board_Manager/package_sparkfun_index.json
ATTENTION !!! Select the 5V, 16Mhz board ONLY !!!

Upload the HEX file:
I don't give sources, because X-Any libraries are not Open Source for the receive side.
Rename Xany2Sounds.ino.with_bootloader.promicro._hex file to Xany2Sounds.ino.with_bootloader.promicro.hex
Pro Micro use a particular method for upload a code.
You can use the program https://github.com/uriba107/32u4_hexuploader .

Configuration mode:
Connect the USB connector of your Pro Micro board to your PC.
Select the console window, define 115200 baud and NL and CR mode.
You have 5 secondes for click on the ENTER key.
When you are in the configuration mode, type h key and ENTER for see all options.
h Help
q quit
5,6,7,9,10,11,12,13,14,15 Xany channel
4 or 8 or 16 Sounds mode
s set shift mode On/Off
c set PWM mode
p set PPM mode
s set SBUS mode
i set IBUS mode
m set SRLX mode
u set SUMD mode
j set JETIEx mode
f set Failsafe values

e reset EEPROM (command hidden)

Add sounds files in your JQ6500 module:
Follow the JQ6500 documentation here, https://sparks.gogo.co.nz/jq6500/index.html .

Good use.

Pierre


