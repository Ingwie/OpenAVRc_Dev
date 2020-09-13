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


#define MAX_SWITCHES 4 // the number of switches
#define DEBOUNCE_TIME 5 // ms delay before the push button changes state

#include "Joystick.h"
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,JOYSTICK_TYPE_JOYSTICK, MAX_SWITCHES, 0, true, true, true, false, false, false, true, true, false, false, false);

HardwareSerial & BT = Serial1;
String str, ch1, ch2, ch3, ch4, ch5, ch6, ch7, ch8;
uint16_t n1, n2, n3, n4, n5, n6, n7, n8;


void setup() {
  Serial.begin(57600);
  BT.begin(57600); //Start Serial BT in 57600
  
  Joystick.begin(false); 
  Joystick.setXAxisRange(-511, 511);
  Joystick.setYAxisRange(-511, 511);
//  Joystick.setZAxisRange(-511, 511);
  Joystick.setRudderRange(-511, 511);
  Joystick.setThrottleRange(0, 1023);
} // END SETUP


void loop() {

  if(BT.available() > 0)
  {
    str = BT.readStringUntil('\n');
    /* Main 4 channels */
    ch1 = getValue(str, 's', 1);Serial.print("ch1: ");n1 = hexToDec(ch1);Serial.println(n1);//Rudder
    ch2 = getValue(str, 's', 2);Serial.print("ch2: ");n2 = hexToDec(ch2);Serial.println(n2);//elevator
    ch3 = getValue(str, 's', 3);Serial.print("ch3: ");n3 = hexToDec(ch3);Serial.println(n3);//throttle
    ch4 = getValue(str, 's', 4);Serial.print("ch4: ");n4 = hexToDec(ch4);Serial.println(n4);//aileron
    n4 = map(n4,1000,2000,-511,511);Joystick.setXAxis(n4);//Aileron
    n2 = map(n2,1000,2000,-511,511);Joystick.setYAxis(n2);//Elevator
    n1 = map(n1,1000,2000,-511,511);Joystick.setRudder(n1);//Rudder
    n3 = map(n3,1000,2000,-511,511);Joystick.setThrottle(n3);//Throttle
    //n5 = map(n5,1000,2000,-511,511);Joystick.setZAxis(n5);

    /* These 4 channels are used as buttons */
    ch5 = getValue(str, 's', 5);Serial.print("ch5: ");n5 = hexToDec(ch5);Serial.println(n5);
    ch6 = getValue(str, 's', 6);Serial.print("ch6: ");n6 = hexToDec(ch6);Serial.println(n6);
    ch7 = getValue(str, 's', 7);Serial.print("ch7: ");n7 = hexToDec(ch7);Serial.println(n7);
    ch8 = getValue(str, 's', 8);ch8 = getValue(ch8, ':', 0);Serial.print("ch8: ");n8 = hexToDec(ch8);Serial.println(n8);Serial.println();       


    if (n5 > 1500) Joystick.pressButton(0); else Joystick.releaseButton(0);
    if (n6 > 1500) Joystick.pressButton(1); else Joystick.releaseButton(1);
    if (n7 > 1500) Joystick.pressButton(2); else Joystick.releaseButton(2);
    if (n8 > 1500) Joystick.pressButton(3); else Joystick.releaseButton(3);
  }
    
	Joystick.sendState();

} // END LOOP


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
