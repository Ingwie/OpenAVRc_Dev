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
//#define CMD_MODE
//#define AT_INIT

/*
How to connect HC05/Pro Micro 16MHz/5v:
 Pro Micro    HC05
    VCC(+5V)   Vcc
    GND        GND
    D8         Tx
    D9 --1K--|-Rx (use 1/2,2k resitors divider betwwen D9 and HC05 RX !)
            2,2K
             |
            GND
    D3(PPM)
    D4 --1K--|-EN (use 1/2,2k resitors divider betwwen D4 and HC05 EN !)
            2,2K
             |
            GND
            
 How to configure HC05:
A)
 - Uncomment #define CMD_MODE in line 34
 - Upload sketch
 - Define console in 115200 bauds
 - Hold KEY button and Power on module
 - Type in console:
   AT (module return OK)
   AT+ORGL (reset module with originals values and return OK)
   AT+NAME=BT/SIM (or what you want, module return OK)
   AT+ROLE=0 (configure in slave mode and return OK)
   AT+PSWD="1234" (use same password in your TX, return OK)
   AT+UART=115200,0,0
 - Comment #define CMD_MODE in line 34 and comment DEBUG in line 33
 - Upload sketch
B)
 - Uncomment #define AT_INIT  in line 35 and comment DEBUG and CMD_MODE
 - Upload sketch
 - Hold Hc05 button if D4 not connected, restart Pro Micro in console mode 38400 baud.
 - If all is 'OK', comment AT_INIT and upload sketch.
 
   Your module is ready :-)
 */

#include <Rcul.h>
#include <TinyPinChange.h>
#include <TinyCppmGen.h>


#define CH_MAX_NB  8
#define CPPM_PERIOD_US        22500


HardwareSerial &BT = Serial1;

#define BT_MSG_MAX_LENGTH      39
//char BtMessage[BT_MSG_MAX_LENGTH + 1];
char BtMessage[] = "tf s77As5F4s3DCs5BDs3DCs5C9s5DEs5D4:0B";
boolean nouvellesDonnees = false;

uint16_t ppmOut[9];

#define PRINT_BUF_SIZE          100
static char PrintBuf[PRINT_BUF_SIZE + 1];

#define PRINTF(fmt, ...)    do{if(Serial){snprintf_P(PrintBuf, PRINT_BUF_SIZE, PSTR(fmt) ,##__VA_ARGS__);Serial.print(PrintBuf);}}while(0)
#define PRINT_P(FlashStr)   do{if(Serial){Serial.print(F(FlashStr));}}while(0)
#define PRINT_W(FlashStr)   do{if(Serial){Serial.print(FlashStr);}}while(0)

#define BIN_NBL_TO_HEX_DIGIT(BinNbl)      ((BinNbl) < 10) ? ((BinNbl) + '0'): ((BinNbl) - 10 + 'A')


#define MODULE_KEY 4


void setup() {

  pinMode(MODULE_KEY, OUTPUT);            // the HC05 Key pin (switch to AT mode when HIGH)
  digitalWrite(MODULE_KEY, LOW);          // communication mode
  
#ifdef DEBUG 
  /*Serial BAUD doit etre moins rapide que celui du BT !!! */
  Serial.begin(115200); //Start Serial monitor in 38400 maxi
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
#endif

#ifdef CMD_MODE
  BT.begin(38400);
  PRINT_P("Start AT commands mode:\r\n\r\n");
#else
  BT.begin(115200);
  PRINT_P("Waiting data from OpenAVRc transmitter\r\n\r\n");
#endif

#ifdef AT_INIT
  BT.begin(38400);
  // programmation automatique du module HC05
  digitalWrite(MODULE_KEY, HIGH);         // switch to AT mode
  Serial.println("AT mode configuration:\r\n");
  delay(2000);
  InitBtAuto();
#endif
/* PPM output pin is imposed by hardware and is target dependant:
   (The user has to define Timer and Channel to use in TinyPpmGen.h file of the library)
       - ATmega328P (Arduino UNO):
         TIMER(0), CHANNEL(A) -> OC0A -> PD6 -> Pin#6
         TIMER(0), CHANNEL(B) -> OC0B -> PD5 -> Pin#5
         TIMER(2), CHANNEL(A) -> OC2A -> PB3 -> Pin#11
         TIMER(2), CHANNEL(B) -> OC2B -> PD3 -> Pin#3
         
       - ATmega32U4 (Arduino Leonardo, Micro and Pro Micro):
         TIMER(0), CHANNEL(A) -> OC0A -> PB7 -> Pin#11 (/!\ pin not available on connector of Pro Micro /!\)
         TIMER(0), CHANNEL(B) -> OC0B -> PD0 -> Pin#3
*/
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

  //https://electroniqueamateur.blogspot.com/2019/10/texte-et-arduino-2-les-tableaux-de.html
  static byte index = 0;
  char charRecu;

  while (BT.available() > 0 && nouvellesDonnees == false) {
    charRecu = BT.read();

    if (charRecu != '\n') { // ce n'est pas la fin du message
      BtMessage[index] = charRecu;
      index++;
      if (index >= BT_MSG_MAX_LENGTH) {
        index = BT_MSG_MAX_LENGTH - 1;
      }
    }
    else {  // c'est la fin du message
      BtMessage[index] = '\0'; // on termine le texte par le caractère nul
      index = 0;
      nouvellesDonnees = true;
    }
  }
  
  if (nouvellesDonnees == true) 
  {
    int init_size = strlen(BtMessage);
    char delim[] = "s:";
    char *ptr = strtok(BtMessage, delim);// Returns first value (here 'tf')

    ppmOut[0] = ptr;//0 return tf

    uint8_t index = 1;
    while(ptr != NULL)//return other values
    {
      //PRINTF("%s\r\n", ptr);
      ptr = strtok(NULL, delim);
      ppmOut[index] = hexToDec(ptr);
      index++;
    }
    
#ifdef DEBUG 
//      PRINT_W(BtMessage);PRINT_P("\r\n");//see https://www.cplusplus.com/reference/cstdio/printf/
      Serial.print(ppmOut[1]);
      Serial.print("\t");Serial.print(ppmOut[2]);
      Serial.print("\t");Serial.print(ppmOut[3]);
      Serial.print("\t");Serial.print(ppmOut[4]);
      Serial.print("\t");Serial.print(ppmOut[5]);
      Serial.print("\t");Serial.print(ppmOut[6]);
      Serial.print("\t");Serial.print(ppmOut[7]);
      Serial.print("\t");Serial.print(ppmOut[8]);
      Serial.print("\t");Serial.println(ppmOut[9]);
//            PRINTF("%u",ppmOut[1]);
//            PRINTF("\t%u",n2);
//            PRINTF("\t%u",n3);
//            PRINTF("\t%u",n4);
//            PRINTF("\t%u",n5);
//            PRINTF("\t%u",n6);
//            PRINTF("\t%u",n7);
//            PRINTF("\t%u",n8);
//            PRINTF("\t%s\r\n",CS);  
#endif
    nouvellesDonnees = false;     
  }
     
  //  Mod1 Dir Prof Gaz Ail
  for (uint8_t i = 1; i < 9 ; i++)
  {
    TinyCppmGen.setChWidth_us(i, ppmOut[i]); //OpenAVRc Trottle
  }      
#endif     
}//END LOOP


//https://github.com/benrugg/Arduino-Hex-Decimal-Conversion/blob/master/hex_dec.ino
uint16_t hexToDec(String hexString) {
  
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
