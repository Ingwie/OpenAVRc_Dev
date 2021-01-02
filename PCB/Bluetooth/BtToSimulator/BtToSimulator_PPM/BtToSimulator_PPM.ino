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
 How to configure HC05:
 - Define console in 38400 bauds
 - Hold KEY button and Power on module
 - Type in console:
   AT (module return OK)
   AT+ORGL (reset module with originals values and return OK)
   AT+NAME=BT/SIM (or what you want, module return OK)
   AT+ROLE=0 (configure in slave mode and return OK)
   AT+PSWD="1234" (use same password in your TX, return OK)
   AT+UART=57600,0,0

   Your module is ready :-)
 */
#define DEBUG
//#define CMD_MODE

#include <Rcul.h>
#include <TinyPinChange.h>

#define CH_MAX_NB  8
#define CPPM_PERIOD_US        22500
#include <TinyCppmGen.h>

//#include <SoftwareSerial.h>
#include <AltSoftSerial.h> //Header File for Serial Bluetooth, will be added by default into Arduino

//SoftwareSerial BT(8,9);// Rx,Tx
AltSoftSerial BT;//Rx=8,Tx=9 for all atmega328

String str = "", ch1, ch2, ch3, ch4, ch5, ch6, ch7, ch8, CS;
uint16_t n1, n2, n3, n4, n5, n6, n7, n8, Checksum;

#define PRINT_BUF_SIZE          100
static char PrintBuf[PRINT_BUF_SIZE + 1];

#define PRINTF(fmt, ...)    do{if(Serial){snprintf_P(PrintBuf, PRINT_BUF_SIZE, PSTR(fmt) ,##__VA_ARGS__);Serial.print(PrintBuf);}}while(0)
#define PRINT_P(FlashStr)   do{if(Serial){Serial.print(F(FlashStr));}}while(0)
#define PRINT_W(FlashStr)   do{if(Serial){Serial.print(FlashStr);}}while(0)

void setup() {
//  pinMode(PIN_MODE,INPUT_PULLUP);
  /*Serial BAUD doit etre moins rapide que celui du BT !!! */
  Serial.begin(38400); //Start Serial monitor in 38400 maxi
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

#ifdef CMD_MODE
    BT.begin(38400);
    PRINT_P("Start AT commands mode:\r\n\r\n");
#else
    BT.begin(57600);
    PRINT_P("Waiting data from OpenAVRc transmitter\r\n\r\n");
#endif
  

//       PPM output pin is imposed by hardware and is target dependant:
//(The user has to define Timer and Channel to use in TinyPpmGen.h file of the library)
//
//           - ATmega328P (Arduino UNO):
//           TIMER(0), CHANNEL(A) -> OC0A -> PD6 -> Pin#6
//           TIMER(0), CHANNEL(B) -> OC0B -> PD5 -> Pin#5
//          >>>>> TIMER(2), CHANNEL(A) -> OC2A -> PB3 -> Pin#11 here is used as output <<<<<
//           TIMER(2), CHANNEL(B) -> OC2B -> PD3 -> Pin#3
  //TinyCppmGen.begin(TINY_CPPM_GEN_POS_MOD, 8);
  TinyCppmGen.begin(TINY_CPPM_GEN_NEG_MOD, CH_MAX_NB, CPPM_PERIOD_US);//Futaba use negative pulse
#ifdef DEBUG  
  PRINT_P("CH1\tCH2\tCH3\tCH4\tCH5\tCH6\tCH7\tCH8\tChecksum\r\n");
#endif
}

void loop() 
{
  
#ifdef CMD_MODE
    if (BT.available())  
    Serial.write(BT.read());

    if (Serial.available())  
    BT.write(Serial.read());
#else

      while(BT.available())
      {
        //tf command format:  sCh1sCh2sCh3sCh4sCh5sCh6sCh7sCh8:CS
  //      tf sHHHsHHHsHHHsHHHsHHHsHHHsHHHsHHH:CS<CR>   (CS=Checksum) -> Ex: tf -100+200-300+400-500+600-700+800:00
        char character = BT.read();
        str.concat(character);
        if (character == '\r'){         
            ch1 = getValue(str, 's', 1);n1 = hexToDec(ch1);
            ch2 = getValue(str, 's', 2);n2 = hexToDec(ch2);
            ch3 = getValue(str, 's', 3);n3 = hexToDec(ch3);
            ch4 = getValue(str, 's', 4);n4 = hexToDec(ch4);
            ch5 = getValue(str, 's', 5);n5 = hexToDec(ch5);
            ch6 = getValue(str, 's', 6);n6 = hexToDec(ch6);
            ch7 = getValue(str, 's', 7);n7 = hexToDec(ch7);
            
            ch8 = getValue(str, 's', 8);
            CS = ch8;
            ch8 = getValue(ch8, ':', 1);
            n8 = hexToDec(ch8);
            CS  = getValue(CS , ':', 2);
            Checksum = hexToDec(CS);

#ifdef DEBUG
//            PRINT_W(str);PRINT_P("\r\n");//see https://www.cplusplus.com/reference/cstdio/printf/
            PRINTF("%u",n1);
            PRINTF("\t%u",n2);
            PRINTF("\t%u",n3);
            PRINTF("\t%u",n4);
            PRINTF("\t%u",n5);
            PRINTF("\t%u",n6);
            PRINTF("\t%u",n7);
            PRINTF("\t%u",n8);
            PRINTF("\t%u\r\n",Checksum);             
#endif
            str = ""; 
        //  Mod1 Dir Prof Gaz Ail
            TinyCppmGen.setChWidth_us(1, n1); //OpenAVRc Trottle
            TinyCppmGen.setChWidth_us(2, n2);
            TinyCppmGen.setChWidth_us(3, n3); 
            TinyCppmGen.setChWidth_us(4, n4);
            TinyCppmGen.setChWidth_us(5, n5); 
            TinyCppmGen.setChWidth_us(6, n6); 
            TinyCppmGen.setChWidth_us(7, n7); 
            TinyCppmGen.setChWidth_us(8, n8);          
        }
      }
#endif
}//END LOOP


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
