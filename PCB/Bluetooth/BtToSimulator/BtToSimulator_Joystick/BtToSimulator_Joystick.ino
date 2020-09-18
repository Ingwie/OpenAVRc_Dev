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
#include <TinyPinChange.h>
#include <TinyCppmGen.h>
#include <Streaming.h>
#include <EEPROM.h>

float VERSION_DECODER = 0.2;

boolean RunConfig = false;
uint8_t mode;
uint32_t LedStartMs=millis();
unsigned long startedWaiting = millis();
unsigned long started1s = millis();
#define LED_SIGNAL_NOTFOUND   1000

#define MAX_SWITCHES 4 // the number of switches
#include "Joystick.h"
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,JOYSTICK_TYPE_JOYSTICK, MAX_SWITCHES, 0, true, true, true, false, false, false, true, true, false, false, false);

HardwareSerial & BT = Serial1;
String str, ch1, ch2, ch3, ch4, ch5, ch6, ch7, ch8;
uint16_t n1, n2, n3, n4, n5, n6, n7, n8;

#define LONGUEUR_MSG_MAX   5              
#define RETOUR_CHARRIOT    0x0D           /* CR (code ASCII) */
#define PASSAGE_LIGNE      0x0A           /* LF (code ASCII) */
#define BACK_SPACE         0x08
char Message[LONGUEUR_MSG_MAX + 1];


void setup() {
  Serial.begin(115200); 
  while (!Serial);// wait for serial port to connect.
  
  Serial << F("Version:")<< EEPROMReadFloat(500) << endl;
  if (EEPROMReadFloat(500) != VERSION_DECODER)
  {
    EEPROM.write(1,1);// PPM
    EEPROMWriteFloat(500,VERSION_DECODER);
  }
  
  BT.begin(57600); //Start Serial BT in 57600

  if (RunConfig == false)
  {
    String sdata="";
    Serial << F("Wait Return");
    byte ch;
    while(millis() - startedWaiting <= 3000) //waiting 3s return key
    { 
      /* Check 1s */
      if(millis()-started1s>=1000)
      {
        Serial << F(".");started1s=millis();
      }
      if(Serial.available() > 0)
      {
        ch = Serial.read();
        sdata += (char)ch;
        if (ch=='\r')
        {
          sdata.trim(); // Process command in sdata.
          sdata = ""; // Clear the string ready for the next command.
          RunConfig = true;
          break;
        }        
      }   
    }
  }

//*********************
//  RunConfig = true;
//*********************

  (RunConfig == true?Serial << endl << endl << F("Configuration mode is actived") << endl:Serial << endl << endl << F("Starting without configuration") << endl);

  mode = EEPROM.read(1);// 1 PPM, 2 JOYSTICK
  
  switch (mode)
  {
    case 1:
      blinkNTime(1,125,250);
      Serial << F("PPM mode in use") << endl;
      if (RunConfig == false)
      {
        /*       PPM output pin is imposed by hardware and is target dependant:
        (The user has to define Timer and Channel to use in TinyPpmGen.h file of the library)
          - ATmega32U4 (Arduino Leonardo, Micro and Pro Micro):
          >>> TIMER(0), CHANNEL(B) -> OC0B -> PD0 -> Pin#3<<<
        */
        TinyCppmGen.begin(TINY_CPPM_GEN_POS_MOD, 8);
        //TinyCppmGen.begin(TINY_CPPM_GEN_NEG_MOD, 8)); 
      }
      break;
    case 2:
      blinkNTime(2,125,250);
      Serial << F("Joystick mode in use") << endl;
      if (RunConfig == false)
      {
        Joystick.begin(false); 
        Joystick.setXAxisRange(-511, 511);
        Joystick.setYAxisRange(-511, 511);
      //  Joystick.setZAxisRange(-511, 511);
        Joystick.setRudderRange(-511, 511);
        Joystick.setThrottleRange(0, 1023); 
      }
      break;
  }

} // END SETUP


void loop() {


  if (RunConfig == true)
  {
      handleSerialDecoder();
      
      //h Help
      //q quit
      //p set PPM mode
      //j set JOYSTICK mode
      //e reset EEPROM (command hidden)   
  }

  if (RunConfig == false)
  { 
    if(BT.available() > 0)
    {
      str = BT.readStringUntil('\n');
      /* Main 4 channels */
      ch1 = getValue(str, 's', 1);Serial.print("ch1: ");n1 = hexToDec(ch1);Serial.println(n1);//Rudder
      ch2 = getValue(str, 's', 2);Serial.print("ch2: ");n2 = hexToDec(ch2);Serial.println(n2);//elevator
      ch3 = getValue(str, 's', 3);Serial.print("ch3: ");n3 = hexToDec(ch3);Serial.println(n3);//throttle
      ch4 = getValue(str, 's', 4);Serial.print("ch4: ");n4 = hexToDec(ch4);Serial.println(n4);//aileron
      ch5 = getValue(str, 's', 5);Serial.print("ch5: ");n5 = hexToDec(ch5);Serial.println(n5);
      ch6 = getValue(str, 's', 6);Serial.print("ch6: ");n6 = hexToDec(ch6);Serial.println(n6);
      ch7 = getValue(str, 's', 7);Serial.print("ch7: ");n7 = hexToDec(ch7);Serial.println(n7);
      ch8 = getValue(str, 's', 8);ch8 = getValue(ch8, ':', 0);Serial.print("ch8: ");n8 = hexToDec(ch8);Serial.println(n8);Serial.println();       
      if (mode == 1)//PPM
      {
        TinyCppmGen.setChWidth_us(1, n1); 
        TinyCppmGen.setChWidth_us(2, n2);
        TinyCppmGen.setChWidth_us(3, n3); 
        TinyCppmGen.setChWidth_us(4, n4);
        TinyCppmGen.setChWidth_us(5, n5); 
        TinyCppmGen.setChWidth_us(6, n6); 
        TinyCppmGen.setChWidth_us(7, n7); 
        TinyCppmGen.setChWidth_us(8, n8);
      }//PPM
      else if (mode == 2)//JOYSTICK
      {   
        n4 = map(n4,1000,2000,-511,511);Joystick.setXAxis(n4);//Aileron
        n2 = map(n2,1000,2000,-511,511);Joystick.setYAxis(n2);//Elevator
        n1 = map(n1,1000,2000,-511,511);Joystick.setRudder(n1);//Rudder
        n3 = map(n3,1000,2000,-511,511);Joystick.setThrottle(n3);//Throttle
        //n5 = map(n5,1000,2000,-511,511);Joystick.setZAxis(n5);    
        if (n5 > 1500) Joystick.pressButton(0); else Joystick.releaseButton(0);
        if (n6 > 1500) Joystick.pressButton(1); else Joystick.releaseButton(1);
        if (n7 > 1500) Joystick.pressButton(2); else Joystick.releaseButton(2);
        if (n8 > 1500) Joystick.pressButton(3); else Joystick.releaseButton(3);
        Joystick.sendState(); 
      }//JOYSTICK
    }
    else
    {
      // Blink each 1s if IBUS not found on Rx pin
      if(millis()-LedStartMs>=LED_SIGNAL_NOTFOUND)
      {
        Serial << F("Waiting BT ...") << endl;
        digitalWrite(17,!digitalRead(17));
        LedStartMs=millis(); // Restart the Chrono for the LED 
      }
    }
  }

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


void handleSerialDecoder() {
  // we only care about two characters to change the pwm
  if (MsgDisponible() >= 0) 
  {
    String mystr = (String)Message;
    uint8_t myInt = mystr.toInt();
    
    if (mystr == "h")
    {
      Serial << F("h Help") << endl;
      Serial << F("q quit") << endl;
      Serial << F("p set PPM mode") << endl;
      Serial << F("j set JOYSTICK mode") << endl;  
    }

//    else if(mystr == "test")
//    {
//      Serial << F("STRING TEST is OK") << endl;
//    }

    else if(mystr == "q")
    {
      Serial << F("Exit  configuration mode") << endl;
      delay(200);
      RunConfig = false;
    }
    else if (myInt > 0 && myInt < 17)
    {
      EEPROM.write(0,myInt);
      Serial << F("PWM canal ") << myInt << (" in use") << endl;  
    }
    else if (mystr == "p")
    {
      Serial << F("Set in PPM mode") << endl;
      EEPROM.write(1,1);  
    }
    else if (mystr == "j")
    {
      Serial << F("Set in JOYSTICK mode") << endl;
      EEPROM.write(1,2);  
    }

  }
}

void waitMs(unsigned long timetowait)
{
  static unsigned long beginTime = 0;
  static unsigned long lastTime = 0;
  beginTime = millis();
  lastTime = beginTime + timetowait;
  do
  {
  }
  while (lastTime > millis());
}


// used for flashing a pin
void blinkNTime(int count, int onInterval, int offInterval)
{
  byte i;
  for (i = 0; i < count; i++) 
  {
    waitMs(offInterval);
    digitalWrite(17,HIGH);
    waitMs(onInterval);
    digitalWrite(17,LOW);  
  }
}

float EEPROMReadFloat(unsigned int addr)
{
  union
  {
    byte b[4];
    float f;
  } data;
  for(int i = 0; i < 4; i++)
  {
    data.b[i] = EEPROM.read(addr+i);
  }
  return data.f;
}
void EEPROMWriteFloat(unsigned int addr, float x)
{
  union
  {
    byte b[4];
    float f;
  } data;
  data.f = x;
  for(int i = 0; i < 4; i++)
  {
    EEPROM.write(addr+i, data.b[i]);
  }
}

int8_t MsgDisponible(void)/* merci a LOUSSOUARN Philippe pour ce code */
{
  int8_t Ret = -1;
  char CaractereRecu;
  static uint8_t Idx = 0;

  
  if(Serial.available() > 0)
  {
    CaractereRecu = Serial.read();
    switch(CaractereRecu)
    {
      case RETOUR_CHARRIOT:
      case PASSAGE_LIGNE:
        Message[Idx] = 0;
        Ret = Idx;
        Idx = 0;
        break; 
      case BACK_SPACE: // Gestion touche d'effacement du dernier caractere sur un terminal fonctionnant caractere par caractere (ex: HyperTerminal, TeraTerm, etc...)
        if(Idx) Idx--;
        break;
      default:
        if(Idx < LONGUEUR_MSG_MAX)
        {
            Message[Idx] = CaractereRecu;
            Idx++;
        }
        else Idx = 0; /* Reset index for the next message */
        break;
    }
  }
  return(Ret); 
}
