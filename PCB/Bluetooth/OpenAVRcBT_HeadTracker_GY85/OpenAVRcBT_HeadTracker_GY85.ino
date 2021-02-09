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

#include "GY_85.h"// see https://github.com/sqrtmo/GY-85-arduino
#include <Wire.h>

GY_85 GY85;     //create the object

HardwareSerial & BT = Serial1;

//#define AT_INIT 
#define DEBUG

float gx,gy,gz,gt;


unsigned long previousMillis = 0;
unsigned long currentMillis = millis();
const long interval = 10;

unsigned long TIMERpreviousMillis = 0;
unsigned long TIMERcurrentMillis = millis();
const long TIMERinterval = 1;

int resetButtonPin = 10;

int buttonCurrent = 0;
int buttonPrevious = 0;
unsigned char buttonHistory = 0;

int test = 0;

uint16_t ppmOut[8];

#ifdef DEBUG
#define PPM_MIN         1000
#define PPM_MAX         2000
#define UP_DIRECTION      +100
#define DOWN_DIRECTION    -100
int pos = PPM_MIN;    // variable to store the servo position 
int step = UP_DIRECTION;
uint32_t PpmSimuStartMs=millis();
#define BT_MSG_MAX_LENGTH      39
char BtMessage[BT_MSG_MAX_LENGTH + 1];
#endif

#define PPM_CENTER      1500
#define NUM_TRAINER     8
int16_t channelOutputs[NUM_TRAINER];
#define FULL_CHANNEL_OUTPUTS(ch) channelOutputs[ch]


/**
* \file  misclib.h
* \fn    Macro: BIN_NBL_TO_HEX_DIGIT(BinNbl)
* \brief returns the ASCII Hexa digit corresponding to a nibble value (eg: BIN_NBL_TO_HEX_DIGIT(15) -> 'F'
* \param BinNbl:  The Nibble value (0 to 15)
*/
#define BIN_NBL_TO_HEX_DIGIT(BinNbl)      ((BinNbl) < 10) ? ((BinNbl) + '0'): ((BinNbl) - 10 + 'A')



//char BtMessage[] = "tf s77As5F4s3DCs5BDs3DCs5C9s5DEs5D4:0B";
uint16_t Tilt,horizontal,vertical;
void setup()
{
  Wire.begin();
  delay(10);
#ifdef DEBUG    
  Serial.begin(115200);
  
#endif
  delay(10);
  GY85.init();
  delay(10);

  BT.begin(115200);  while (!BT);// wait for serial port to connect.

}

void loop()
{
  headTracking();
  BT_Send_Channels();
}



void headTracking()
{
  TIMERcurrentMillis = millis();
  if (TIMERcurrentMillis - TIMERpreviousMillis >= TIMERinterval)
  {
    //reset timer
    TIMERpreviousMillis = TIMERcurrentMillis;

    buttonPrevious = buttonCurrent;
    if(digitalRead(resetButtonPin) == HIGH)
    {
      buttonCurrent = 1;
    }
    else
    {
      buttonCurrent = 0;
    }
    buttonHistory = (buttonHistory << 1) | buttonCurrent;

    if((buttonPrevious == 0)&&(buttonCurrent == 1))
    {
      test = 0;
    }
  }

  if(test == 0)
  {
    if(buttonHistory == 0x0)
    {
      test = 1;
      
      gx = 0;
      gy = 0;
      gz = 0;


    }
  }

  currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {    
//    int ax = GY85.accelerometer_x( GY85.readFromAccelerometer() );
//    int ay = GY85.accelerometer_y( GY85.readFromAccelerometer() );
//    int az = GY85.accelerometer_z( GY85.readFromAccelerometer() );
//    
    int cx = GY85.compass_x( GY85.readFromCompass() );
    int cy = GY85.compass_y( GY85.readFromCompass() );
    int cz = GY85.compass_z( GY85.readFromCompass() );

    gx = GY85.gyro_x( GY85.readGyro() );
    gy = GY85.gyro_y( GY85.readGyro() );
    gz = GY85.gyro_z( GY85.readGyro() );
    gt = GY85.temp  ( GY85.readGyro() );
    
#ifdef DEBUG    
//    Serial.print  ( "accelerometer" );
//    Serial.print  ( " x:" );
//    Serial.print  ( ax );
//    Serial.print  ( " y:" );
//    Serial.print  ( ay );
//    Serial.print  ( " z:" );
//    Serial.print  ( az );
//    
//    Serial.print  ( "  compass" );
//    Serial.print  ( " x:" );
//    Serial.print  ( cx );
//    Serial.print  ( " y:" );
//    Serial.print  ( cy );
//    Serial.print  (" z:");
//    Serial.print  ( cz );
    
    //Serial.print  ( "  gyro" );
//    Serial.print  ( gx );
//    Serial.print  ( "," );
//    Serial.print  ( gy );
//    Serial.print  ( "," );
//    Serial.println  ( gz );
//    Serial.print  ( "," );
//    Serial.println( gt );
#endif
    
    horizontal = map(cx,-600,600,1000,2000);ppmOut[5] = horizontal;
    vertical = map(cy,-600,600,1000,2000);ppmOut[6] = vertical;
    Tilt = map(cz,-600,600,1000,2000);ppmOut[7] = Tilt;
    
    //reset timer
    previousMillis = currentMillis;
  }      

}

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
  
#ifdef DEBUG
  //Serial.print(bt);
  int bt_len = bt.length() + 1;
  bt.toCharArray(BtMessage, bt_len);
  
  //check BT message send
//  uint8_t RxChks, ComputedChks;
//  ComputedChks = 0;
//  for(uint8_t Idx = 0; Idx < (8 * 4); Idx++) // Il y a 8 voies et chaque voie fait 4 caractères ->
//  {
//    ComputedChks ^= BtMessage[3 + Idx]; // On commence après un offset de 3 caracteres (apres "tf ")
//  }
//  RxChks = (uint8_t)strtol(BtMessage + 3 + (8 * 4) + 1, NULL, 16);
//  if(RxChks == ComputedChks)
//  {
//    Serial.println(" (Good Checksum OK)");
//  }
//  else
//  {
//    Serial.println(" (Bad Checksum OK)");
//  }
#endif  

}

#ifdef DEBUG
uint16_t GetChannelValueUs(uint8_t ChId) // ChId va de 1 a 8
{
  uint8_t  ChIdx = ChId - 1;
  uint16_t ChVal = 1500;

  if((ChId >= 1) && (ChId <= 8))
  {
    BtMessage[3 + (ChId * 4)] = 0; // Replace 's' by \0 (End of string)
    ChVal = (uint16_t)strtol(BtMessage + 3 + ((ChId - 1) * 4) + 1, NULL, 16);
  }
  return(ChVal);
}
#endif

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
