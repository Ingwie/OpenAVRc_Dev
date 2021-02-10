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
#define PPM         0
#define BLUETOOTH    1
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
#include <Rcul.h>
#include <TinyPinChange.h>
#include <TinyCppmGen.h>

//PPM config values
#define FRAME_LENGTH 22500  //set the PPM frame length in microseconds (1ms = 1000µs)
#define PULSE_LENGTH 300  //set the pulse length
#define onState 1  //set polarity of the pulses: 1 is positive, 0 is negative
#define sigPin 3  //set PPM signal output pin on the arduino

#endif

BtnPPMMap btnPPMMap;
USB                                             Usb;
USBHub                                          Hub(&Usb);
HIDUniversal                                    Hid(&Usb);
JoystickEvents                                  JoyEvents(&btnPPMMap);
JoystickReportParser                            Joy(&JoyEvents);

#if defined(__AVR_ATmega328P__)
#include <SoftwareSerial.h>
SoftwareSerial BT(7,8);// RX, TX use 57600 maxi
#endif
#if defined(__AVR_ATmega32U4__) 
HardwareSerial & BT = Serial1;// Only with Leonardo board
#endif

//#include <TinyCppmGen.h>
//#include <Rcul.h>
#define CPPM_PERIOD_US  22500
#define PPM_CENTER      1500
#define NUM_TRAINER     8
int16_t channelOutputs[NUM_TRAINER];
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

  pinMode(sigPin, OUTPUT);
//  digitalWrite(sigPin, !onState);  //set the PPM signal pin to the default state (off)

#if defined(__AVR_ATmega328P__)  
  BT.begin(57600);  while (!BT);// wait for serial port to connect.
#endif
#if defined(__AVR_ATmega32U4__)  
  BT.begin(115200);  while (!BT);// wait for serial port to connect.
#endif

#if (MODE == PPM)
  TinyCppmGen.begin(TINY_CPPM_GEN_NEG_MOD, NUM_TRAINER, CPPM_PERIOD_US); /* Change CTINY_PPM_GEN_POS_MOD to TINY_CPPM_GEN_NEG_MOD for NEGative CPPM modulation */

/*
  cli();
  TCCR1A = 0; // set entire TCCR1 register to 0
  TCCR1B = 0;

  OCR1A = 100;  // compare match register, change this
  TCCR1B |= (1 << WGM12);  // turn on CTC mode
  TCCR1B |= (1 << CS11);  // 8 prescaler: 0,5 microseconds at 16mhz
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
  sei();
*/
#endif
}

void loop()
{
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
//  Serial.print(ppmOut[1]);
//  Serial.print("\t");Serial.print(ppmOut[2]);
//  Serial.print("\t");Serial.print(ppmOut[3]);
//  Serial.print("\t");Serial.print(ppmOut[4]);
//  Serial.print("\t");Serial.print(ppmOut[5]);
//  Serial.print("\t");Serial.print(ppmOut[6]);
//  Serial.print("\t");Serial.print(ppmOut[7]);
//  Serial.print("\t");Serial.println(ppmOut[8]);
  //btnPPMMap.debug();

#if (MODE == PPM)
	for (uint8_t i = 0; i < 8 ; i++)
	{
	  TinyCppmGen.setChWidth_us(i+1, ppmOut[i]); //OpenAVRc Trottle
	} 
#endif
#if (MODE == BLUETOOTH)
  BT_Send_Channels();
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
 
  for(uint8_t Idx = 0; Idx < NUM_TRAINER; Idx++)
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

#if (MODE == BLUETOOTH)
#ifdef AT_INIT          // AT configuration of the HC05, to make once time
void InitBtAuto()
{
  Serial.print("AT: ");                 // verify we are in AT mode
  BT.print("AT\r\n");
  if (!waitFor("OK\r\n")) Serial.println("time out error AT");

  Serial.print("UART: ");               // serial communication parameters
  BT.print("AT+UART=115200,0,0\r\n");
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

//  Serial.print("RMAAD: ");              // Delete all authenticated devices in the pair list
//  BT.print("AT+RMAAD\r\n");
//  if (!waitFor("OK\r\n")) Serial.println("time out error AT+RMAAD");

//  Serial.print("CMODE: ");              // connection mode
//  BT.print("AT+CMODE=0\r\n");
//  if (!waitFor("OK\r\n")) Serial.println("time out error AT+CMODE");

  digitalWrite(MODULE_KEY, LOW);            // leave AT mode & switch back to communication mode
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
#endif

#if (MODE == PPM)
/*
ISR(TIMER1_COMPA_vect) { //leave this alone
  static boolean state = true;

  TCNT1 = 0;

  if (state) {  //start pulse
    digitalWrite(sigPin, onState);
    OCR1A = PULSE_LENGTH * 2;
    state = false;
  } else { //end pulse and calculate when to start the next pulse
    static byte cur_chan_numb;
    int channelValue;
    static unsigned int calc_rest;

    digitalWrite(sigPin, !onState);
    state = true;

    if (cur_chan_numb >= BtnPPMMap::NUM_CHANNELS) {
      cur_chan_numb = 0;
      calc_rest = calc_rest + PULSE_LENGTH;//
      OCR1A = (FRAME_LENGTH - calc_rest) * 2;
      calc_rest = 0;
    }
    else {
      channelValue = btnPPMMap.getChannelValue(cur_chan_numb);
      OCR1A = (channelValue - PULSE_LENGTH) * 2;
      calc_rest = calc_rest + channelValue;
      cur_chan_numb++;
    }
  }
*/
#endif