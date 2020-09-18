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


#include <Rcul.h>
#include <TinyCppmReader.h>// configurer le le pour atmega328

#define NUM_TRAINER     8
#define PPM_INPUT_PIN   3// PPM Input
#define PPM_CENTER      1500
#define PPM_CH_CENTER(ch) (PPM_CENTER)
#define NUM_CHNOUT      16
int16_t channelOutputs[NUM_CHNOUT];
#define FULL_CHANNEL_OUTPUTS(ch) channelOutputs[ch]

//#define DEBUG
#ifdef DEBUG
String str, ch1, ch2, ch3, ch4, ch5, ch6, ch7, ch8;
uint16_t n1, n2, n3, n4, n5, n6, n7, n8;
#endif

#define UP_DIRECTION      +100
#define DOWN_DIRECTION    -100
#define POS_MIN           1000
#define POS_MAX           2000
int pos = POS_MIN;    // variable to store the servo position 
int step = UP_DIRECTION;
uint32_t PpmSimuStartMs=millis();

/**
* \file  misclib.h
* \fn    Macro: BIN_NBL_TO_HEX_DIGIT(BinNbl)
* \brief returns the ASCII Hexa digit corresponding to a nibble value (eg: BIN_NBL_TO_HEX_DIGIT(15) -> 'F'
* \param BinNbl:  The Nibble value (0 to 15)
*/
#define BIN_NBL_TO_HEX_DIGIT(BinNbl)      ((BinNbl) < 10) ? ((BinNbl) + '0'): ((BinNbl) - 10 + 'A')

HardwareSerial & BT = Serial1;
TinyCppmReader TinyCppmReader; 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);  while (!Serial);// wait for serial port to connect.
  BT.begin(115200);  while (!BT);// wait for serial port to connect.

  TinyCppmReader.attach(PPM_INPUT_PIN); // Attach TinyPpmReader to PPM_INPUT_PIN pin
  
  for(uint8_t Idx = 0; Idx < NUM_TRAINER; Idx++)
  {
    FULL_CHANNEL_OUTPUTS(Idx) = map(1500,1000,2000,-1280,1280);
  }

}

void loop() {
  if (TinyCppmReader.isSynchro())
  {
    for(uint8_t Idx = 0; Idx < NUM_TRAINER; Idx++)
    {
      FULL_CHANNEL_OUTPUTS(Idx) = map(TinyCppmReader.width_us(Idx+1),1000,2000,-1280,1280);
    }
  }
  else
  {
    if(millis()-PpmSimuStartMs>=250)
    {
      pos += step;Serial.print(F("Simulation replace PPM signal: "));Serial.println(pos);
      if(pos >= POS_MAX) step = DOWN_DIRECTION; //180 degrees reached -> Change direction
      if(pos <= POS_MIN) step = UP_DIRECTION;   //  0 degrees reached -> Change direction
      for(uint8_t Idx = 0; Idx < NUM_TRAINER; Idx++)
      {
        FULL_CHANNEL_OUTPUTS(Idx) = map(pos,1000,2000,-1280,1280);
      } 
      PpmSimuStartMs=millis(); /* Restart the Chrono for the LED */
    }  
  }
  BT_Send_Channels();
}


void BT_Send_Channels()
{
 char txt;
 uint8_t ComputedCheckSum = 0;

 BT.print(F("tf "));
#ifdef DEBUG
 str = "tf ";
#endif

 for(uint8_t Idx = 0; Idx < NUM_TRAINER; Idx++)
  {
#ifdef DEBUG
   str += "s";
#endif
   BT.print('s');
   int16_t value = (FULL_CHANNEL_OUTPUTS(Idx))/2; // +-1280 to +-640
   value += PPM_CENTER; // + 1500 offset
   ComputedCheckSum ^= 's';
   value <<= 4;
   for(uint8_t j = 12; j ; j-=4)
    {
     txt = BIN_NBL_TO_HEX_DIGIT((value>>j) & 0x0F);
     ComputedCheckSum ^= txt;
     BT.print(txt);
#ifdef DEBUG
     str += txt;
#endif
    }
  }
#ifdef DEBUG
  str += ":";
#endif
  BT.print(':');
  txt = BIN_NBL_TO_HEX_DIGIT(ComputedCheckSum>>4 & 0x0F);
#ifdef DEBUG
  str += txt;
#endif
  BT.print(txt);
  txt = BIN_NBL_TO_HEX_DIGIT(ComputedCheckSum & 0x0F);
#ifdef DEBUG
  str += txt;Serial.println(str);
#endif
  BT.println(txt);
  

#ifdef DEBUG
  ch1 = getValue(str, 's', 1);Serial.print("ch1: ");n1 = hexToDec(ch1);Serial.println(n1);
  ch2 = getValue(str, 's', 2);Serial.print("ch2: ");n2 = hexToDec(ch2);Serial.println(n2);
  ch3 = getValue(str, 's', 3);Serial.print("ch3: ");n3 = hexToDec(ch3);Serial.println(n3);
  ch4 = getValue(str, 's', 4);Serial.print("ch4: ");n4 = hexToDec(ch4);Serial.println(n4);
  ch5 = getValue(str, 's', 5);Serial.print("ch5: ");n5 = hexToDec(ch5);Serial.println(n5);
  ch6 = getValue(str, 's', 6);Serial.print("ch6: ");n6 = hexToDec(ch6);Serial.println(n6);
  ch7 = getValue(str, 's', 7);Serial.print("ch7: ");n7 = hexToDec(ch7);Serial.println(n7);
  ch8 = getValue(str, 's', 8);ch8 = getValue(ch8, ':', 0);Serial.print("ch8: ");n8 = hexToDec(ch8);Serial.println(n8);Serial.println();       
#endif
}

#ifdef DEBUG
/* BT FORMAT DECOD TEST */
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

//https://github.com/benrugg/Arduino-Hex-Decimal-Conversion/blob/master/hex_dec.ino
unsigned int hexToDec(String hexString) {
  
  unsigned int decValue = 0;
  int nextInt;
  
  for (int i = 0; i < hexString.length(); i++) {
    
    nextInt = int(hexString.charAt(i));
    if (nextInt >= 48 && nextInt <= 57) nextInt = map(nextInt, 48, 57, 0, 9);
    if (nextInt >= 65 && nextInt <= 70) nextInt = map(nextInt, 65, 70, 10, 15);
    if (nextInt >= 97 && nextInt <= 102) nextInt = map(nextInt, 97, 102, 10, 15);
    nextInt = constrain(nextInt, 0, 15);
    
    decValue = (decValue * 16) + nextInt;
  }
  
  return decValue;
}
#endif
