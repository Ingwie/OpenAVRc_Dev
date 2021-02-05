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

//#define DEBUG
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

#define MODE JOYSTICK //Select PPM or JOYSTICK

//#if (MODE == PPM)
#include <Rcul.h>
#include <TinyPinChange.h>
#include <TinyCppmGen.h>
//#else if (MODE == JOYSTICK)
//#include "Joystick.h"

//#define INCLUDE_X_AXIS true
//#define INCLUDE_Y_AXIS true
//#define INCLUDE_Z_AXIS true
//#define INCLUDE_RX_AXIS true
//#define INCLUDE_RY_AXIS true
//#define INCLUDE_RZ_AXIS true
//
//#define HAT_SWITCH_COUNT 1  // Indicates how many hat switches will be available on the joystick. (0-2)
//#define MAX_SWITCHES 4 // the number of switches
//
//#define INCLUDE_RUDDER true  // Indicates if the Rudder is available on the joystick.
//#define INCLUDE_THROTTLE true  // Indicates if the Throttle is available on the joystick.
//#define INCLUDE_ACCELERATOR false  // Indicates if the Accelerator is available on the joystick.
//#define INCLUDE_BRAKE true  // Indicates if the Brake is available on the joystick.
//#define INCLUDE_STEERING false  // Indicates if the Steering is available on the joystick.

//Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_JOYSTICK, MAX_SWITCHES, HAT_SWITCH_COUNT,
//                     INCLUDE_X_AXIS, INCLUDE_Y_AXIS, INCLUDE_Z_AXIS, INCLUDE_RX_AXIS,
//                     INCLUDE_RY_AXIS, INCLUDE_RZ_AXIS, INCLUDE_RUDDER, INCLUDE_THROTTLE,
//                     INCLUDE_ACCELERATOR, INCLUDE_BRAKE, INCLUDE_STEERING);
//Joystick_ Joystick;
//#endif

#define CH_MAX_NB  8
#define CPPM_PERIOD_US        225000


HardwareSerial &BT = Serial1;

#define BT_MSG_MAX_LENGTH      39
char BtMessage[BT_MSG_MAX_LENGTH + 1];
//char BtMessage[] = "tf s77As5F4s3DCs5BDs3DCs5C9s5DEs5D4:0B";
boolean nouvellesDonnees = false;

uint8_t goodValue = 0;
uint8_t newValueCounter = 0;
  
uint16_t ppmOut[9];

#define PRINT_BUF_SIZE          100
static char PrintBuf[PRINT_BUF_SIZE + 1];

#define PRINTF(fmt, ...)        snprintf_P(PrintBuf, PRINT_BUF_SIZE, PSTR(fmt) ,##__VA_ARGS__);Serial.print(PrintBuf)
#define PRINT_P(FlashStr)   do{if(Serial){Serial.print(F(FlashStr));}}while(0)

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

//#if (MODE == PPM)
//    //TinyCppmGen.begin(TINY_CPPM_GEN_POS_MOD, 8);
    TinyCppmGen.begin(TINY_CPPM_GEN_NEG_MOD, CH_MAX_NB, CPPM_PERIOD_US);//Futaba use negative pulse
//#else if (MODE == JOYSTICK)
//    Joystick.begin(false); 
//  Joystick.setXAxisRange(-127, 127);
//  Joystick.setYAxisRange(-127, 127);
//  Joystick.setZAxisRange(-127, 127);
//  Joystick.setRxAxisRange(0, 360);
//  Joystick.setRyAxisRange(360, 0);
//  Joystick.setRzAxisRange(0, 720);
//  Joystick.setThrottleRange(0, 255);
//  Joystick.setRudderRange(255, 0);
//#endif
  
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
  static uint8_t ComputedCheckSum;
  
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


  //nouvellesDonnees = true;// uncomment for test only
  if (nouvellesDonnees == true) 
  {
    //Serial.println(BtMessage);
    newValueCounter += 1;
    uint8_t RxChks, ComputedChks;
    ComputedChks = 0;
    for(uint8_t Idx = 0; Idx < (8 * 4); Idx++) // Il y a 8 voies et chaque voie fait 4 caractères ->
    {
      ComputedChks ^= BtMessage[3 + Idx]; // On commence après un offset de 3 caracteres (apres "tf ")
    }
    RxChks = (uint8_t)strtol(BtMessage + 3 + (8 * 4) + 1, NULL, 16);
    //PRINTF("RxChks=0x%02X -> Computed Chks=0x%02X -> %s\n", RxChks, ComputedChks, (RxChks == ComputedChks)? "OK": "KO");
    //Serial.print("Counter: ");Serial.println(newValueCounter);
    if(RxChks == ComputedChks)
    {
      goodValue += 1;
      //Serial.print("Good: ");Serial.println(goodValue);
      for(uint8_t ChId = 1; ChId <= 8; ChId++)
      {
        //PRINTF("Ch[%u]=%04u\n", ChId, GetChannelValueUs(ChId));
        ppmOut[ChId] = GetChannelValueUs(ChId);
#ifdef DEBUG
        Serial.print(ppmOut[1]);
        Serial.print("\t");Serial.print(ppmOut[2]);
        Serial.print("\t");Serial.print(ppmOut[3]);
        Serial.print("\t");Serial.print(ppmOut[4]);
        Serial.print("\t");Serial.print(ppmOut[5]);
        Serial.print("\t");Serial.print(ppmOut[6]);
        Serial.print("\t");Serial.print(ppmOut[7]);
        Serial.print("\t");Serial.println(ppmOut[8]);
//        PRINTF("%u",ppmOut[1]);
//        PRINTF("\t%u",ppmOut[2]);
//        PRINTF("\t%u",ppmOut[3]);
//        PRINTF("\t%u",ppmOut[4]);
//        PRINTF("\t%u",ppmOut[5]);
//        PRINTF("\t%u",ppmOut[6]);
//        PRINTF("\t%u",ppmOut[7]);
//        PRINTF("\t%u\r\n",ppmOut[8]);
#endif       
      }
    }
    nouvellesDonnees = false;     
  }
  if (newValueCounter == 100)
  {
    //PRINTF("Taux err = %u \r\n",goodValue);
    newValueCounter = 0;
    goodValue = 0;   
  }

//#if (MODE == PPM)
    //  Mod1 Dir Prof Gaz Ail
    for (uint8_t i = 1; i < 9 ; i++)
    {
      TinyCppmGen.setChWidth_us(i, ppmOut[i]); //OpenAVRc Trottle
    }        
//#else if (MODE == JOYSTICK)

//    ppmOut[1] = map(ppmOut[1],1000,2000,0,255);Joystick.setThrottle(ppmOut[1]);//Throttle
//    ppmOut[2] = map(ppmOut[2],1000,2000,-127,127);Joystick.setXAxis(ppmOut[2]);//Aileron
//    ppmOut[3] = map(ppmOut[3],1000,2000,-127,127);Joystick.setYAxis(ppmOut[3]);//Elevator
//    ppmOut[4] = map(ppmOut[4],1000,2000,-127,127);Joystick.setRudder(ppmOut[4]);//Rudder
//    //n5 = map(n5,1000,2000,-511,511);Joystick.setZAxis(n5);    
//    if (ppmOut[5] > 1500) Joystick.pressButton(0); else Joystick.releaseButton(0);
//    if (ppmOut[6] > 1500) Joystick.pressButton(1); else Joystick.releaseButton(1);
//    if (ppmOut[7] > 1500) Joystick.pressButton(2); else Joystick.releaseButton(2);
//    if (ppmOut[8] > 1500) Joystick.pressButton(3); else Joystick.releaseButton(3);
//    Joystick.sendState();     
//#endif
     
#endif     
}//END LOOP

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

//void testHatSwitch(unsigned int currentStep)
//{
//  if (currentStep < 8)
//  {
//    Joystick.setHatSwitch(0, currentStep * 45);
//  }
//  else if (currentStep == 8)
//  {
//    Joystick.setHatSwitch(0, -1);
//  }
//  else if (currentStep < 17)
//  {
//    Joystick.setHatSwitch(1, (currentStep - 9) * 45);
//  }
//  else if (currentStep == 17)
//  {
//    Joystick.setHatSwitch(1, -1);
//  }
//  else if (currentStep == 18)
//  {
//    Joystick.setHatSwitch(0, 0);
//    Joystick.setHatSwitch(1, 0);
//  }
//  else if (currentStep < 27)
//  {
//    Joystick.setHatSwitch(0, (currentStep - 18) * 45);
//    Joystick.setHatSwitch(1, (8 - (currentStep - 18)) * 45);
//  }
//  else if (currentStep == 27)
//  {
//    Joystick.setHatSwitch(0, -1);
//    Joystick.setHatSwitch(1, -1);
//  }
//}

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
