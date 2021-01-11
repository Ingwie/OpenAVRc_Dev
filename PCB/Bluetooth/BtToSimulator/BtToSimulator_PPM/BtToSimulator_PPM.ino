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

#define LONGUEUR_MSG_MAX   50             /* ex: tf sHHHsHHHsHHHsHHHsHHHsHHHsHHHsHHH:CS<CR> */
#define RETOUR_CHARRIOT    0x0D           /* CR (code ASCII) */
#define PASSAGE_LIGNE      0x0A           /* LF (code ASCII) */
#define BACK_SPACE         0x08
char Message[LONGUEUR_MSG_MAX + 1];
uint8_t SubStrNb, SeparFound;
#define SUB_STRING_NB_MAX  8              /* nombre de valeurs splitées */
char *StrTbl[SUB_STRING_NB_MAX];          /* declaration de pointeurs sur chaine, 1 pointeur = 2 octets seulement */


/*
How to connect HC05/Pro Micro 16MHz/5v:
 Pro Mini     HC05
    VCC(+5V)   Vcc
    GND        GND
    D8         Tx
    D9 --1K--|-Rx (use 1/2,2k resitors divider betwwen D9 and HC05 RX !)
            2,2K
             |
            GND
    D3(PPM)
 
 How to configure HC05:
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
   
 - Comment #define CMD_MODE in line 34
 - Upload sketch
   Your module is ready :-)
 */

#include <Rcul.h>
#include <TinyPinChange.h>

#define CH_MAX_NB  4
#define PULSE_WIDTH_MIN_US    1000
#define PULSE_WIDTH_MAX_US    2000
#define CPPM_PERIOD_US        20000
#include <TinyCppmGen.h>

HardwareSerial &BT = Serial1;

String str = "", ch1, ch2, ch3, ch4, ch5, ch6, ch7, ch8, CS;
uint16_t n1, n2, n3, n4, n5, n6, n7, n8, Checksum;
uint16_t ppmOut[9];

#define PRINT_BUF_SIZE          100
static char PrintBuf[PRINT_BUF_SIZE + 1];

#define PRINTF(fmt, ...)    do{if(Serial){snprintf_P(PrintBuf, PRINT_BUF_SIZE, PSTR(fmt) ,##__VA_ARGS__);Serial.print(PrintBuf);}}while(0)
#define PRINT_P(FlashStr)   do{if(Serial){Serial.print(F(FlashStr));}}while(0)
#define PRINT_W(FlashStr)   do{if(Serial){Serial.print(FlashStr);}}while(0)

#define BIN_NBL_TO_HEX_DIGIT(BinNbl)      ((BinNbl) < 10) ? ((BinNbl) + '0'): ((BinNbl) - 10 + 'A')

void setup() {
  /*Serial BAUD doit etre moins rapide que celui du BT !!! */
  Serial.begin(115200); //Start Serial monitor in 38400 maxi
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

#ifdef CMD_MODE
    BT.begin(38400);
    PRINT_P("Start AT commands mode:\r\n\r\n");
#else
    BT.begin(115200);
    PRINT_P("Waiting data from OpenAVRc transmitter\r\n\r\n");
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
//  TinyCppmGen.begin(TINY_CPPM_GEN_NEG_MOD, 8);
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

  ReadMessageFromBT();
//#ifdef DEBUG
//  PRINTF("%u",ppmOut[1]);
//  PRINTF("\t%u",ppmOut[2]);
//  PRINTF("\t%u",ppmOut[3]);
//  PRINTF("\t%u",ppmOut[4]);
//  PRINTF("\t%u",ppmOut[5]);
//  PRINTF("\t%u",ppmOut[6]);
//  PRINTF("\t%u",ppmOut[7]);
//  PRINTF("\t%u",ppmOut[8]);
//  PRINTF("\t%x\r\n",ppmOut[9]);
//#endif    
      //  Mod1 Dir Prof Gaz Ail
  if (TinyCppmGen.isSynchro())
  {
    for (uint8_t i = 1; i < 9 ;i++)
    {
      TinyCppmGen.setChWidth_us(i, ppmOut[i]);
    }  
  }

  delay(10);
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

void ReadMessageFromBT() {
  if (MsgDisponible() >= 0) 
  {
    String mystr = (String)Message;
    mystr.replace("\r\n","\0");
#ifdef DEBUG
//    PRINT_W(mystr);PRINT_P("\r\n");
#endif
    if (strlen(Message) == 39)
    {
      static String checkMess;     
      StrSplit(Message, "s",  StrTbl, SUB_STRING_NB_MAX, &SeparFound);
      checkMess = StrTbl[0];
//      PRINT_W(checkMess);PRINT_P("\r\n");
      if (checkMess == "tf ")
      {
        for (uint8_t p = 1; p < 9 ; p++)
        {
         //ppmOut[p] = constrain(hexToDec(StrTbl[p]),PULSE_WIDTH_MIN_US,PULSE_WIDTH_MAX_US);
         ppmOut[p] = hexToDec(StrTbl[p]);        
        }
        StrSplitRestore("s", StrTbl, SeparFound);//Imperatif SeparFound <= SUB_STRING_NB_MAX
        StrSplit(Message, ":",  StrTbl, SUB_STRING_NB_MAX, &SeparFound);
        ppmOut[9] = StrTbl[1];
//#ifdef DEBUG
//        PRINTF("%u",ppmOut[1]);
//        PRINTF("\t%u",ppmOut[2]);
//        PRINTF("\t%u",ppmOut[3]);
//        PRINTF("\t%u",ppmOut[4]);
//        PRINTF("\t%u",ppmOut[5]);
//        PRINTF("\t%u",ppmOut[6]);
//        PRINTF("\t%u",ppmOut[7]);
//        PRINTF("\t%u",ppmOut[8]);
//        PRINTF("\t%x\r\n",ppmOut[9]);
//#endif 
      }       
    }
  }
}

int8_t StrSplit(char *SrcStr, char *Separ,  char **TarStrTbl, uint8_t TblLenMax, uint8_t *SeparFound)/* merci a LOUSSOUARN Philippe pour ce code */
{
  uint8_t SubStrFound, SeparLen;
  char *StartStr, *SeparPos;

  *SeparFound = 0;
  SubStrFound = 0;
  /* Clear Table Entries (secu) in case of empty string */
  for(uint8_t Idx = 0; Idx < TblLenMax; Idx++)
  {
    TarStrTbl[Idx] = NULL; /* End of string */
  }
  if (!strlen(SrcStr)) return(0);
  StartStr = SrcStr; /* Do NOT touch SrcStr */
  SeparLen = strlen(Separ);
  while(TblLenMax)
  {
    SeparPos = strstr(StartStr, Separ);
    if(SeparPos)
    {
      /* OK Substring found */
      *SeparPos = 0;
      TarStrTbl[(*SeparFound)++] = StartStr;
      StartStr = SeparPos + SeparLen;
      SubStrFound++; // <---- Correction d'un bug
    }
    else
    {
      SubStrFound = *SeparFound;
      if(*StartStr)
      {
        TarStrTbl[*SeparFound] = StartStr;
        SubStrFound++;
      }
      break;
    }
    TblLenMax--;
  }
  return(SubStrFound);
}

int8_t StrSplitRestore(char *Separ,  char **TarStrTbl, uint8_t SeparNbToRestore)/* merci a LOUSSOUARN Philippe pour ce code */
{
  uint8_t Idx, Restored = 0;
  
  for(Idx = 0; Idx < SeparNbToRestore; Idx++)
  {
    /* Restore \0 with first char of Separator */
    if(TarStrTbl[Idx] != NULL)
    {
      TarStrTbl[Idx][strlen(TarStrTbl[Idx])] = Separ[0];
      Restored++;
    }
  }
  return(Restored);
}

int8_t MsgDisponible(void)/* merci a LOUSSOUARN Philippe pour ce code */
{
  int8_t Ret = -1;
  char CaractereRecu;
  static uint8_t Idx = 0;

  
  if(BT.available() > 0)
  {
    CaractereRecu = BT.read();
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
