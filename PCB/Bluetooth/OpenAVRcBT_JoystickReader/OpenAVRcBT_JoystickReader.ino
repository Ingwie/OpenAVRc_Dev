/*
**************************************************************************
*                                                                        *
*                 ____                ___ _   _____                      *
*                / __ \___  ___ ___  / _ | | / / _ \____                 *
*               / /_/ / _ \/ -_) _ \/ __ | |/ / , _/ __/                 *
*               \____/ .__/\__/_//_/_/ |_|___/_/|_|\__/                  *
*                   /_/                                                  *
*                                                                        *
*              This file is part of the OpenAVRc project.                *
*                                                                        *
*                         Based on code(s) named :                       *
*             OpenTx - https://github.com/opentx/opentx                  *
*             Deviation - https://www.deviationtx.com/                   *
*                                                                        *
*                Only AVR code here for visibility ;-)                   *
*                                                                        *
*   OpenAVRc is free software: you can redistribute it and/or modify     *
*   it under the terms of the GNU General Public License as published by *
*   the Free Software Foundation, either version 2 of the License, or    *
*   (at your option) any later version.                                  *
*                                                                        *
*   OpenAVRc is distributed in the hope that it will be useful,          *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of       *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
*   GNU General Public License for more details.                         *
*                                                                        *
*       License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html          *
*                                                                        *
************************************************************************** 
*/

/* 
 Simplified Logitech Extreme 3D Pro Joystick Report Parser 
 Original code by lexfp https://github.com/lexfp/le3dpToPPM
*/

/*
 Command an OpenAVRc radio with a Logitech 3D Pro Joystick over a HC-05
*/

//#define AT_INIT
#define DEBUG

#define PPM         0
#define BLUETOOTH   1
#define MODE BLUETOOTH //Select PPM or BLUETOOTH

#include <usbhid.h>
#include <hiduniversal.h>
#include <usbhub.h>

#include "le3dp_rptparser.h"

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

#if (MODE == PPM)
#include "PPMEncoder.h"
#define PPM_OUTPUT_PIN 4
#endif

#if (MODE == BLUETOOTH)
uint16_t BLUETOOTH_BAUDS;
#if defined(__AVR_ATmega328P__)
#include <SoftwareSerial.h>
SoftwareSerial BT(7,8);// RX, TX use 57600 maxi
#endif
#if defined(__AVR_ATmega32U4__) 
HardwareSerial & BT = Serial1;// Only with Leonardo board
#endif
#endif

BtnPPMMap btnPPMMap;
USB                                             Usb;
USBHub                                          Hub(&Usb);
HIDUniversal                                    Hid(&Usb);
JoystickEvents                                  JoyEvents(&btnPPMMap);
JoystickReportParser                            Joy(&JoyEvents);

int16_t channelOutputs[BtnPPMMap::NUM_CHANNELS];
#define FULL_CHANNEL_OUTPUTS(ch) channelOutputs[ch]
uint16_t ppmOut[8];

/**
* \file  misclib.h
* \fn    Macro: BIN_NBL_TO_HEX_DIGIT(BinNbl)
* \brief returns the ASCII Hexa digit corresponding to a nibble value (eg: BIN_NBL_TO_HEX_DIGIT(15) -> 'F'
* \param BinNbl:  The Nibble value (0 to 15)
*/
#define BIN_NBL_TO_HEX_DIGIT(BinNbl)      ((BinNbl) < 10) ? ((BinNbl) + '0'): ((BinNbl) - 10 + 'A')

void setup()
{
 
  Serial.begin( 115200 );
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  Serial.println("Start");

  if (Usb.Init() == -1)
    Serial.println("OSC did not start.");

  delay( 200 );

  if (!Hid.SetReportParser(0, &Joy))
    ErrorMessage<uint8_t>(PSTR("SetReportParser"), 1  );

#if (MODE == PPM)
  ppmEncoder.begin(PPM_OUTPUT_PIN);
#endif

#ifdef AT_INIT
  #undef MODE
  Serial.begin( 9600 );//need to be < BT speed in this mode
  BT.begin(38400);while (!BT);
  InitBtAuto();
  delay(500);
  ReadBTSettings();
#endif
  
#if (MODE == BLUETOOTH)
#if defined(__AVR_ATmega328P__)  
  BT.begin(57600);  while (!BT);// wait for serial port to connect.
#endif
#if defined(__AVR_ATmega32U4__)  
  BT.begin(115200);  while (!BT);// wait for serial port to connect.
#endif
#endif

}


void loop()
{

#ifdef AT_INIT
  if (BT.available())  
  Serial.write(BT.read());

  if (Serial.available())  
  BT.write(Serial.read());
#else  
  Usb.Task();
  if (Usb.getUsbTaskState() != USB_STATE_RUNNING)
  {
    //disconnected joystick
    JoyEvents.disconnectJoystick();
    //Serial.println("Joystick disconnected");
  }

  for (uint8_t ch = 0; ch < 8 ; ch++)
  {
    ppmOut[ch] = btnPPMMap.getChannelValue(ch);
  }
  
#ifdef DEBUG
  btnPPMMap.debug();
#endif

#if (MODE == PPM)  
	for (uint8_t i = 0; i < 8 ; i++)
	{
		ppmEncoder.setChannel(i, ppmOut[i]);
		ppmEncoder.setChannel(i, BtnPPMMap::PPM_MIN_VALUE);
		ppmEncoder.setChannelPercent(0, 0);
	  
		// Max value
		ppmEncoder.setChannel(i, ppmOut[i]);
		ppmEncoder.setChannel(i, BtnPPMMap::PPM_MAX_VALUE);
		ppmEncoder.setChannelPercent(0, 100);
	} 
#endif

#if (MODE == BLUETOOTH)
  BT_Send_Channels();
#endif

#endif
}

#if (MODE == BLUETOOTH)
void BT_Send_Channels()
{
  char txt;
  uint8_t ComputedCheckSum = 0;
  String bt;
  BT.print(F("tf "));
  bt += "tf ";
 
  for(uint8_t Idx = 0; Idx < BtnPPMMap::NUM_CHANNELS; Idx++)
  {

   BT.print('s');
   bt += "s";
   int16_t value = (FULL_CHANNEL_OUTPUTS(Idx))/2; // +-1280 to +-640
   value += ppmOut[Idx];//PPM_CENTER; // + 1500 offset
   ComputedCheckSum ^= 's';
   value <<= 4;
   for(uint8_t j = 12; j ; j-=4)
    {
     txt = BIN_NBL_TO_HEX_DIGIT((value>>j) & 0x0F);
     bt += (String)txt;
     ComputedCheckSum ^= txt;
     BT.print(txt);
    }
  }

  BT.print(':');
  bt += ":";
  txt = BIN_NBL_TO_HEX_DIGIT(ComputedCheckSum>>4 & 0x0F);
  bt += (String)txt;
  BT.print(txt);
  txt = BIN_NBL_TO_HEX_DIGIT(ComputedCheckSum & 0x0F);
  bt += (String)txt;
  BT.println(txt);
}
#endif


#ifdef AT_INIT          // AT configuration of the HC05, to make once time
void InitBtAuto()
{
  Serial.print("AT: ");                 // verify we are in AT mode
  BT.print("AT\r\n");
  if (!waitFor("OK\r\n")) Serial.println("time out error AT");

  Serial.print("UART: ");               // serial communication parameters
#if defined(__AVR_ATmega328P__)
  BT.print("AT+UART=57600,0,0\r\n");//57600
#endif
#if defined(__AVR_ATmega32U4__) 
  BT.print("AT+UART=115200,0,0\r\n");//115200
#endif
  if (!waitFor("OK\r\n")) Serial.println("time out error AT+UART");

  Serial.print("NAME: ");               // bluetooth device name
  BT.print("AT+NAME=BT/SIM\r\n");
  if (!waitFor("OK\r\n")) Serial.println("time out error AT+NAME");

  Serial.print("PSWD: ");               // password for pairing
  BT.print("AT+PSWD=\"1234\"\r\n");
  if (!waitFor("OK\r\n")) Serial.println("time out error AT+PSWD");

  Serial.print("ROLE: ");               // device in slave mode
  BT.print("AT+ROLE=0\r\n");
  if (!waitFor("OK\r\n")) Serial.println("time out error AT+ROLE");

//  digitalWrite(MODULE_KEY, LOW);            // leave AT mode & switch back to communication mode
}

void ReadBTSettings()
{
  BT.print("AT\r\n");

  BT.print("AT+UART\r\n");

  BT.print("AT+NAME\r\n");

  BT.print("AT+PSWD\r\n");

  BT.print("AT+ROLE\r\n");
}

//  The waitFor a string function with time out
bool waitFor(const char *rep){
unsigned long t0 = millis(), t;
int i=0, j=strlen(rep);
char c;

  for (i = 0 ; i<j ; )
  {
    if (BT.available())
    {
      c=(char)BT.read();
      Serial.print(c);
      if (c==rep[i])i++;
    }

    t = millis();
    if (t-t0>2000)break;
  }
  return i==j ;
  delay(10);
}

#endif
