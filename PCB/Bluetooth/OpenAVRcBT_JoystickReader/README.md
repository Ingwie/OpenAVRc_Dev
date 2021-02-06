# Logitech Extreme 3d pro to PPM
Use your Logitech joystick to fly quads/fixed wing.
While this does work with crossfire, I would not use it to fly near people or do long range (unless you're flying over a desert where no one can get hurt).
This is not well tested software, so use at your own risk. Please use your common sense and do not use this in a way that can potentially cause injuries to others. 

 https://youtu.be/3cB8Vwtuyw8

## Requirements
Logitech Extreme 3d pro joystick
Arduino Uno (or equivalent clone)
Arduino USB Shield
PPM Driven TX module (see below)

## Installation
Search for tutorials for on up Arduino so you can at least download sketches and upload them to your arduino. You will also need to be able to install arduino libraries such as the usb host shield library here:   
https://github.com/felis/USB_Host_Shield_2.0/archive/master.zip  
I will not cover any of these steps as they are covered in depth in many other places if you search for it.
The only two things I will cover are for cheap cloned boards from China. The clones will often use a different driver (CH340). Lots of sites will cover how to install this.  
The clone usb shields may need some additional jumpers bridged or they will not work.   
See https://esp8266-notes.blogspot.com/2017/08/defective-arduino-usb-host-shield-boards.html  
Contrary to the article, these boards are not defective, but rather they just don't come fully configured and ready to go from the factory. Welcome to the hobby!

## Wiring
See wiring.jpg image in codebase.  
With the module lights and dials facing you and the antenna facing up, you will find 5 pins on the bottom right. The top pin on the module will go to pin 3 on your arduino board (you can change that in the code). Pins 3 on the module is for your battery + and pin 4 is ground. I wired them directly to my arduino battery terminal so that everything is powered once I power on the arduino. I believe that they can both take 3s although I've opted to use 2s. You can disregard the other pins.

## Configuration
The code has the configuration mapped to what suits me. It can, however, be easily changed with very basic programming knowledge to do what you want. Learn how to use the serial monitor tool in arduino will allow you to figure out the numbers assigned to the buttons on your joystick which can be mapped (uncomment print statements in JoystickEvents::OnGamePadChanged) By changing some of the if statements below, you will be able to remap the different functions for arming, disarming, and other modes. You can also add other modes/functionality to suit your needs as well. 

By default, button 2 is for turtle mode, button 3 is for acro and 4 is for horizon. Arming is button 12 and disarming is button 11. Throttle is done with the slider and the rest is obvious (yaw done with twist).  
The following are my modes in betaflight:  
aux 0 0 0 1300 1700 0 0  
aux 1 2 1 1300 1700 0 0  
aux 2 35 1 1875 2100 0 0  
Please note that if the joystick is unplugged at any time, the code will disarm the quad (assuming the modes are set accordingly). It is probably a better idea to set this to a failsafe value instead if you fly anything other than a whoop. Just remember to set up the flight control software (betaflight, inav, etc...) to failsafe on a particular channel value.

## Transmitter Modules
The modules linked below should work, but in general, if you can operate the module in opentx with PPM mode, it should work. I tested with the module in the first link and also the crossfire module.  
https://www.team-blacksheep.com/products/prod:crossfire_micro_tx  
https://www.banggood.com/2_4G-CC2500-NRF24L01-A7105-CTRF6936-4-IN-1-Multi-protocol-STM32-TX-Module-With-Antenna-p-1153902.html?p=Y506095296511201607R&custlinkid=1443521  
https://www.banggood.com/IRangeX-IRX4-Plus-2_4G-CC2500-NRF24L01-A7105-CYRF6936-4-IN-1-Multiprotocol-STM32-TX-Module-With-Case-p-1225080.html?p=Y506095296511201607R&custlinkid=1443522  

## Inspired by
https://github.com/DroneMesh/USB_PPM  
Code is, however, based off of the le3dp example in the shield library and the ppm encoder code by David Hasko.  
https://github.com/felis/USB_Host_Shield_2.0/tree/master/examples/HID/le3dp  
https://github.com/DzikuVx/ppm_encoder  
