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
#define DEBUG

#include <Rcul.h>
#include <TinyPinChange.h>
#include <TinyCppmGen.h>
#include <SoftSerial.h> //Header File for Serial Bluetooth, will be added by default into Arduino

SoftSerial BT(8,9);// Rx,Tx

String str, ch1, ch2, ch3, ch4, ch5, ch6, ch7, ch8;
uint16_t n1, n2, n3, n4, n5, n6, n7, n8;
void setup() {

  Serial.begin(57600); //Start Serial monitor in 9600
  Serial.println(F("Waiting data from OpenAVRc transmitter"));
  BT.begin(57600); //Name of your Bluetooth Signal

//       PPM output pin is imposed by hardware and is target dependant:
//(The user has to define Timer and Channel to use in TinyPpmGen.h file of the library)
//
//           - ATmega328P (Arduino UNO):
//           TIMER(0), CHANNEL(A) -> OC0A -> PD6 -> Pin#6
//           TIMER(0), CHANNEL(B) -> OC0B -> PD5 -> Pin#5
//          >>>>> TIMER(2), CHANNEL(A) -> OC2A -> PB3 -> Pin#11 here is used as output <<<<<
//           TIMER(2), CHANNEL(B) -> OC2B -> PD3 -> Pin#3 

//           - ATmega32U4 (Arduino Leonardo, Micro and Pro Micro):
//           TIMER(0), CHANNEL(A) -> OC0A -> PB7 -> Pin#11 (/!\ pin not available on connector of Pro Micro /!\)
//           TIMER(0), CHANNEL(B) -> OC0B -> PD0 -> Pin#3
  TinyCppmGen.begin(TINY_CPPM_GEN_POS_MOD, 8);
  //TinyCppmGen.begin(TINY_CPPM_GEN_NEG_MOD, 8));
  
}

void loop() 
{
  if(BT.available() > 0)
  {
    str = BT.readStringUntil('\n');
    ch1 = getValue(str, 's', 1);n1 = hexToDec(ch1);
    ch2 = getValue(str, 's', 2);n2 = hexToDec(ch2);
    ch3 = getValue(str, 's', 3);n3 = hexToDec(ch3);
    ch4 = getValue(str, 's', 4);n4 = hexToDec(ch4);
    ch5 = getValue(str, 's', 5);n5 = hexToDec(ch5);
    ch6 = getValue(str, 's', 6);n6 = hexToDec(ch6);
    ch7 = getValue(str, 's', 7);n7 = hexToDec(ch7);
    ch8 = getValue(str, 's', 8);ch8 = getValue(ch8, ':', 0);n8 = hexToDec(ch8);       
#ifdef DEBUG
    Serial.println(str);
    Serial.print("ch1: ");Serial.println(n1);
    Serial.print("ch2: ");Serial.println(n2);
    Serial.print("ch3: ");Serial.println(n3);
    Serial.print("ch4: ");Serial.println(n4);
    Serial.print("ch5: ");Serial.println(n5);
    Serial.print("ch6: ");Serial.println(n6);
    Serial.print("ch7: ");Serial.println(n7);
    Serial.print("ch8: ");Serial.println(n8);Serial.println();
#endif
    TinyCppmGen.setChWidth_us(1, n1); 
    TinyCppmGen.setChWidth_us(2, n2);
    TinyCppmGen.setChWidth_us(3, n3); 
    TinyCppmGen.setChWidth_us(4, n4);
    TinyCppmGen.setChWidth_us(5, n5); 
    TinyCppmGen.setChWidth_us(6, n6); 
    TinyCppmGen.setChWidth_us(7, n7); 
    TinyCppmGen.setChWidth_us(8, n8);
  }
  else
  {
#ifdef DEBUG
    Serial.println(F("BT data not found, default values are send !"));
#endif    
  }
    TinyCppmGen.setChWidth_us(1, 1500); 
    TinyCppmGen.setChWidth_us(2, 1500);
    TinyCppmGen.setChWidth_us(3, 1000); 
    TinyCppmGen.setChWidth_us(4, 1500);
    TinyCppmGen.setChWidth_us(5, 1500); 
    TinyCppmGen.setChWidth_us(6, 1500); 
    TinyCppmGen.setChWidth_us(7, 1500); 
    TinyCppmGen.setChWidth_us(8, 1500); 
}

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
