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

#include "bluetooth.h"

#define BT_MASTER     1 // TO DO

extern void checkMixer(void);

#define BT_SEND_AT_SEQ(AtCmdInit)     btSendAtSeq((const AtCmdSt_t*)&AtCmdInit, TBL_ITEM_NB(AtCmdInit))

DECL_FLASH_STR2(Str_CRLF,        "\r\n");
DECL_FLASH_STR2(Str_OK_CRLF,     "OK\r\n");
DECL_FLASH_STR2(Str_CRLF_OK_CRLF,"\r\nOK\r\n");

DECL_FLASH_STR2(Str_STATE,       "+STATE?");// BT Status
DECL_FLASH_STR2(Str_UART_,       "+UART");  // Uart
DECL_FLASH_STR2(Str_CLASS_,      "+CLASS"); // Device Class
DECL_FLASH_STR2(Str_RMAAD,       "+RMAAD"); // Clears paired list
DECL_FLASH_STR2(Str_ROLE_,       "+ROLE");  // Role: 0=Slave, 1=Master
DECL_FLASH_STR2(Str_NAME_,       "+NAME");  // BT module name
DECL_FLASH_STR2(Str_RNAME_,      "+RNAME"); // Remote BT module name
DECL_FLASH_STR2(Str_RESET,       "+RESET"); // Reset
DECL_FLASH_STR2(Str_CMODE_,      "+CMODE"); // Inquire - Connection mode
DECL_FLASH_STR2(Str_INIT,        "+INIT");  // Initialize the SPP profile lib
DECL_FLASH_STR2(Str_INQ,         "+INQ");   // Inquire Bluetooth device

typedef void (*AtCmdAddon) (char* Addon);

typedef struct{
  const char *Cmd;
  AtCmdAddon  CmdAddon;
  const char *TermPattern;
  uint8_t     TermPatternNb;
  uint16_t    TimeoutMs;
}AtCmdSt_t;

#define BT_READ_TIMEOUT_MS         60
#define BT_WRITE_TIMEOUT_MS        100
#define BT_SCANN_TIMEOUT_MS        20000
#define BT_READ_RNAME_TIMEOUT_MS   10000

enum {OFF = 0, ON};

static char      *getAtCmd(const AtCmdSt_t *AtCmdTbl, uint8_t Idx, char *Buf);
static AtCmdAddon getAtCmdAddon(const AtCmdSt_t *AtCmdTbl, uint8_t Idx);
static char      *getAtTermPattern(const AtCmdSt_t *AtCmdTbl, uint8_t Idx, char *Buf);
static uint8_t    getAtTermPatternNb(const AtCmdSt_t *AtCmdTbl, uint8_t Idx);
static uint16_t   getAtTimeoutMs(const AtCmdSt_t *AtCmdTbl, uint8_t Idx);

static void    AtCmdMode(uint8_t On);
static void    uartSet(char* Addon);
static void    classSet(char* Addon);
static void    genGet(char* Addon);
static void    roleSet(char* Addon);
static void    nameSet(char* Addon);
static void    inqGet(char* Addon);
static void    inqSet(char* Addon);
static void    rnameGet(char* Addon);

static uint8_t getSerialMsg(char *TermPattern = "\r", uint8_t TermPatternNb = 1, uint16_t TimeoutMs = 0);
#define ASYNC_GET_SERIAL_MSG()          getSerialMsg()
#define SYNC_GET_SERIAL_MSG(TimeoutMs)  getSerialMsg(TimeoutMs)

static void    btSendAtSeq(const AtCmdSt_t *AtCmdTbl, uint8_t TblItemNb);

DECL_FLASH_TBL(AtCmdBtInit, AtCmdSt_t) = {
                          /* Cmd      CmdAddon TermPattern  TermPatternNb  TimeoutMs */
                          {NULL,      NULL,    Str_CRLF,         1,     BT_READ_TIMEOUT_MS},
                          {Str_UART_, uartSet, Str_CRLF,         1,     BT_WRITE_TIMEOUT_MS},
                          {Str_CLASS_,classSet,Str_CRLF,         1,     BT_WRITE_TIMEOUT_MS},
                          {Str_INQ,   inqSet,  Str_CRLF,         1,     BT_WRITE_TIMEOUT_MS},
                          };

DECL_FLASH_TBL(AtCmdSlaveInit, AtCmdSt_t) = {
                          /* Cmd      CmdAddon TermPattern  TermPatternNb  TimeoutMs */
                          {NULL,      NULL,    Str_CRLF,         1,     BT_READ_TIMEOUT_MS},
                          {Str_ROLE_, roleSet, Str_CRLF,         1,     BT_WRITE_TIMEOUT_MS},
                          {Str_ROLE_, genGet,  Str_CRLF_OK_CRLF, 1,     BT_READ_TIMEOUT_MS},
                          {Str_NAME_, nameSet, Str_CRLF,         1,     BT_WRITE_TIMEOUT_MS},
                          {Str_NAME_, genGet,  Str_CRLF_OK_CRLF, 1,     BT_READ_TIMEOUT_MS},
                          };

DECL_FLASH_TBL(AtCmdMasterInit, AtCmdSt_t) = {
                          /* Cmd      CmdAddon TermPattern  TermPatternNb  TimeoutMs */
                          {NULL,      NULL,    Str_CRLF,         1,     BT_READ_TIMEOUT_MS},
                          {Str_RMAAD, NULL,    Str_CRLF,         1,     BT_WRITE_TIMEOUT_MS},
                          {Str_ROLE_, roleSet, Str_CRLF,         1,     BT_WRITE_TIMEOUT_MS},
                          {Str_ROLE_, genGet,  Str_CRLF_OK_CRLF, 1,     BT_READ_TIMEOUT_MS},
                          {Str_NAME_, nameSet, Str_CRLF,         1,     BT_WRITE_TIMEOUT_MS},
                          {Str_NAME_, genGet,  Str_CRLF_OK_CRLF, 1,     BT_READ_TIMEOUT_MS},
                          {Str_INQ,   inqGet,  Str_CRLF_OK_CRLF, 1,     BT_READ_TIMEOUT_MS},
                          {Str_INIT,  NULL,    Str_CRLF,         1,     BT_WRITE_TIMEOUT_MS},
                          {Str_INQ,   NULL,    Str_CRLF,         3,     BT_SCANN_TIMEOUT_MS},
                          {Str_RNAME_,rnameGet,Str_CRLF_OK_CRLF, 1,     BT_READ_RNAME_TIMEOUT_MS},
                          };

void bluetooth_init(HwSerial *hwSerial)
{
  uint32_t RateTbl[] = {115200, 57600, 38400, 19200, 9600};
  uint8_t  Idx;
  char     UartAtCmd[30];

  AtCmdMode(ON);
  for(Idx = 0; Idx < TBL_ITEM_NB(RateTbl); Idx++)
  {
    hwSerial->init(RateTbl[Idx]);
    while(hwSerial->available()) hwSerial->read(); // Flush Rx
    hwSerial->print(F("AT\r\n"));
    if(getSerialMsg((char *)"K\r\n", 1, 100))
    {
      /* OK Uart serial rate found */
//Serial.print(F("BT rate found: "));Serial.println(RateTbl[Idx]);
      if(Idx)
      {
        sprintf_P(UartAtCmd, PSTR("AT+UART=%lu,0,0\r\n"), RateTbl[0]);
//Serial.print(F("Set BT rate to: "));Serial.println(UartAtCmd);
        hwSerial->print(UartAtCmd);
        if(getSerialMsg((char *)"\r\n", 1, 100))
        {
          hwSerial->init(RateTbl[0]);
          // BT Reboot is needed
          // TO DO
        }
      }
      break;
    }
    else
    {
//      Serial.print(F("No BT resp at: "));Serial.println(RateTbl[Idx]);
    }
  }
  AtCmdMode(OFF);
}


static void AtCmdMode(uint8_t On)
{
  uint32_t StartDurationMs;

//  digitalWrite(BT_KEY_EN, On);
  if(On)
  {
    YIELD_TO_TASK_FOR_MS(checkMixer(), StartDurationMs, 60);
  }
}

static char   *getAtCmd(const AtCmdSt_t *AtCmdTbl, uint8_t Idx, char *Buf)
{
  char *StrAddr;

  *Buf = 0;
  StrAddr = (char*)pgm_read_word(&AtCmdTbl[Idx].Cmd);
  if(StrAddr)
  {
    strcpy_P(Buf, StrAddr);
  }
  return(Buf);
}

static AtCmdAddon getAtCmdAddon(const AtCmdSt_t *AtCmdTbl, uint8_t Idx)
{
  return((AtCmdAddon)pgm_read_word(&AtCmdTbl[Idx].CmdAddon));
}

static char   *getAtTermPattern(const AtCmdSt_t *AtCmdTbl, uint8_t Idx, char *Buf)
{
  strcpy_P(Buf, (char*)pgm_read_word(&AtCmdTbl[Idx].TermPattern));
  return(Buf);
}

static uint8_t getAtTermPatternNb(const AtCmdSt_t *AtCmdTbl, uint8_t Idx)
{
  return((uint8_t)pgm_read_byte(&AtCmdTbl[Idx].TermPatternNb));
}

static uint16_t getAtTimeoutMs(const AtCmdSt_t *AtCmdTbl, uint8_t Idx)
{
  return((uint16_t)pgm_read_word(&AtCmdTbl[Idx].TimeoutMs));
}

static uint8_t getSerialMsg(char *TermPattern, uint8_t TermPatternNb, uint16_t TimeoutMs)
{
  uint32_t StartMs = GET_10MS_TICK();
  uint8_t  RxChar, TermPatternLen, RxLen = 0;
  static int8_t Pidx = -1; /* Shall be static if getSerialMsg is called asynchronously */

  TermPatternLen = strlen(TermPattern);
  do
  {
    YIELD_TO_TASK(checkMixer());
    if(uCli.stream->available() >  0)
    {
      RxChar = uCli.stream->read();
//Serial.print(F("Rx: '"));Serial.print(RxChar);Serial.println(F("'"));
      if(RxChar == BACK_SPACE)
      {
        if(uCli.CmdLine.Idx) uCli.CmdLine.Idx--;
      }
      else
      {
        if(Pidx < 0)
        {
          /* No match caugth yet */
          if(RxChar == TermPattern[Pidx + 1])
          {
            Pidx++;
            goto AddChar; // Sometimes goto helps! :-p
          }
        }
        else
        {
          /* Match in progress */
          if(RxChar == TermPattern[Pidx + 1])
          {
            Pidx++;
          }
          else Pidx = -1; /* Match broken */
        }
        AddChar:
        if(uCli.CmdLine.Idx < CMD_LINE_MAX_SIZE)
        {
          uCli.CmdLine.Msg[uCli.CmdLine.Idx++] = RxChar;
        }
        else
        {
          uCli.CmdLine.Idx = 0; //msg too long!
          Pidx = -1;
        }
        if(Pidx >= (TermPatternLen - 1))
        {
          /* Full pattern found -> replace it by End of String */
//Serial.print("uCli.CmdLine.Idx=");Serial.print(uCli.CmdLine.Idx);Serial.print(" TermPatternLen=");Serial.println(TermPatternLen);
          TermPatternNb--;
          if(!TermPatternNb)
          {
            uCli.CmdLine.Idx -= TermPatternLen;
            uCli.CmdLine.Msg[uCli.CmdLine.Idx] = 0;
            RxLen = uCli.CmdLine.Idx;
            uCli.CmdLine.Idx = 0;
            Pidx = -1;
          }
        }
      }
    }
  }while((GET_10MS_TICK() - StartMs) < MS_TO_10MS_TICK(TimeoutMs) && !RxLen);

  return(RxLen);
}

static void btSendAtSeq(const AtCmdSt_t *AtCmdTbl, uint8_t TblItemNb)
{
  uint8_t    Idx, TermPatternNb;
  uint16_t   TimeoutMs;
  AtCmdAddon CmdAddon;
  char       Buf[30];

  AtCmdMode(ON);
  for(Idx = 0; Idx < TblItemNb; Idx++)
  {
    CmdAddon      = getAtCmdAddon(AtCmdTbl,  Idx);
    TermPatternNb = getAtTermPatternNb(AtCmdTbl, Idx);
    TimeoutMs     = getAtTimeoutMs(AtCmdTbl, Idx);
//Serial.print(F("<-"));
    uCli.stream->print(F("AT"));
//Serial.print(F("'AT"));
//Serial.print(getAtCmd(AtCmdTbl, Idx, Buf));
    uCli.stream->print(getAtCmd(AtCmdTbl, Idx, Buf));
    if(CmdAddon)
    {
      CmdAddon(Buf);
//Serial.print(Buf);
      uCli.stream->print(Buf);
    }
    uCli.stream->print(F("\r\n"));
//Serial.print(F("'\r\n"));
    getAtTermPattern(AtCmdTbl, Idx, Buf); // Buf contains now TermPattern
//Serial.print(F("->'"));
    if(getSerialMsg(Buf, TermPatternNb, TimeoutMs))
    {
//Serial.print(uCli.CmdLine.Msg);Serial.println(F("'"));
    }
//else Serial.println(F("No reponse!'"));
  }
  AtCmdMode(OFF);
}

static void uartSet(char* Addon)
{
  strcpy_P(Addon, PSTR("=115200,0,0"));
}

static void classSet(char* Addon)
{
  strcpy_P(Addon, PSTR("=0"));
}

static void genGet(char* Addon)
{
  Addon[0] = '?';
  Addon[1] =  0;
}

static void roleSet(char* Addon)
{
  Addon[0] = '=';
  Addon[1] = '0' + BT_MASTER;
  Addon[2] =  0;
}

static void inqGet(char* Addon)
{
  strcpy_P(Addon, PSTR("M?"));
}

static void inqSet(char* Addon)
{
  strcpy_P(Addon, PSTR("M=0,4,4")); // 4 devices max, during 4 x 1.28 sec
}

static void nameSet(char* Addon)
{
  strcpy(Addon, "=RC-NAVY_");
  strcat(Addon, (BT_MASTER == 1)? "M": "S");
}

static void rnameGet(char* Addon)
{
  strcpy(Addon, "?25,56,D8CA0F");
}

uint8_t setAtCmdAndWaitForResp(char *AtCmd, uint16_t TimeoutMs)
{
  uCli.stream->print(AtCmd);
  uCli.stream->print(F("\r\n"));
  return(0);
}
