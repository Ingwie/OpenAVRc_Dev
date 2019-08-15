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

#define BT_MASTER     1 // Replace this by eeprom BT bit

#define BT_SEND_AT_SEQ(AtCmdInit)     btSendAtSeq((const AtCmdSt_t*)&AtCmdInit, TBL_ITEM_NB(AtCmdInit))

#define PRIO_TASK_LIST()              checkMixer()

enum {BT_GET = 0, BT_SET};
enum {OFF = 0, ON};

DECL_FLASH_STR2(Str_CRLF,"\r\n");
DECL_FLASH_STR2(Str_OK_CRLF,"OK\r\n");
DECL_FLASH_STR2(Str_CRLF_OK_CRLF,"\r\nOK\r\n");

DECL_FLASH_STR2(Str_AT,    "");      // Simple AT command
DECL_FLASH_STR2(Str_STATE, "STATE"); // BT Status
DECL_FLASH_STR2(Str_PSWD,  "PSWD");  // BT Password (PIN)
DECL_FLASH_STR2(Str_UART,  "UART");  // Uart
DECL_FLASH_STR2(Str_CLASS, "CLASS"); // Device Class
DECL_FLASH_STR2(Str_RMAAD, "RMAAD"); // Clears paired list
DECL_FLASH_STR2(Str_ROLE,  "ROLE");  // Role: 0=Slave, 1=Master
DECL_FLASH_STR2(Str_NAME,  "NAME");  // BT module name
DECL_FLASH_STR2(Str_RNAME, "RNAME"); // Remote BT module name
DECL_FLASH_STR2(Str_CMODE, "CMODE"); // Inquire - Connection mode
DECL_FLASH_STR2(Str_INIT,  "INIT");  // Initialize the SPP profile lib
DECL_FLASH_STR2(Str_INQM,  "INQM");  // Inquire Mode
DECL_FLASH_STR2(Str_INQ,   "INQ");   // Inquire Bluetooth device
DECL_FLASH_STR2(Str_RESET, "RESET"); // Reset

enum {AT_AT = 0, AT_STATE, AT_PSWD, AT_UART, AT_CLASS, AT_RMAAD, AT_ROLE, AT_NAME, AT_RNAME, AT_CMODE, AT_INIT, AT_INQM, AT_INQ, AT_CMD_MAX_NB};

DECL_FLASH_TBL(AtCmdTbl, char * const)  = {Str_AT, Str_STATE, Str_PSWD, Str_UART, Str_CLASS, Str_RMAAD, Str_ROLE, Str_NAME, Str_RNAME, Str_CMODE, Str_INIT, Str_INQM, Str_INQ, Str_RESET};

typedef void (*AtCmdAddon) (char* Addon);

typedef struct{
  uint8_t     CmdIdx;
  uint8_t     BtOp;
  AtCmdAddon  CmdAddon;
  const char *TermPattern;
  uint8_t     MatchLen;
  uint8_t     SkipLen;
  uint16_t    TimeoutMs;
}AtCmdSt_t;

/* PRIVATE FUNCTION PROTOTYPES */
static void    AtCmdMode(uint8_t On);
static uint8_t getAtCmdIdx(const AtCmdSt_t *AtCmdTbl, uint8_t Idx);
static char   *getAtCmd(uint8_t Idx, char *Buf);
static uint8_t getAtMatchLen(const AtCmdSt_t *AtCmdTbl, uint8_t Idx);
static uint8_t getAtSkipLen(const AtCmdSt_t *AtCmdTbl, uint8_t Idx);
static uint8_t getAtBtOp(const AtCmdSt_t *AtCmdTbl, uint8_t Idx);
static int8_t  setAtCmdAndWaitForResp(uint8_t AtCmdIdx, uint8_t BtOp, char *AtCmdArg, char *Resp, uint8_t MatchLen, uint8_t SkipLen, char *TermPattern, uint16_t TimeoutMs);
static int8_t  getSerialMsg(char *TermPattern, uint8_t TermPatternNb, uint16_t TimeoutMs);
static void    btSendAtSeq(const AtCmdSt_t *AtCmdTbl, uint8_t TblItemNb);
static char   *buidMacStr(char *MacBin, char *MacStr);

static void    uartSet(char* Addon);
static void    classSet(char* Addon);
static void    roleSet(char* Addon);
static void    nameSet(char* Addon);
static void    inqmSet(char* Addon);

#define BT_GET_TIMEOUT_MS          60
#define BT_SET_TIMEOUT_MS          100
#define BT_SCANN_TIMEOUT_MS        20000
#define BT_READ_RNAME_TIMEOUT_MS   10000

const AtCmdSt_t AtCmdBtInit[] PROGMEM = {
                          /* CmdIdx,  BtOp,  CmdAddon, TermPattern  MatchLen, SkipLen, TimeoutMs */
                          {AT_AT,    BT_GET, NULL,     Str_CRLF,          0,    0,     BT_GET_TIMEOUT_MS},
                          {AT_UART,  BT_SET, uartSet,  Str_CRLF,          0,    0,     BT_SET_TIMEOUT_MS},
                          {AT_CLASS, BT_SET, classSet, Str_CRLF,          0,    0,     BT_SET_TIMEOUT_MS},
                          {AT_INQM,  BT_SET, inqmSet,  Str_CRLF,          0,    0,     BT_SET_TIMEOUT_MS},
                          };

const AtCmdSt_t AtCmdSlaveInit[] PROGMEM = {
                          /* CmdIdx,  BtOp,  CmdAddon, TermPattern  MatchLen, SkipLen, TimeoutMs */
                          {AT_AT,    BT_GET, NULL,    Str_CRLF,           0,    0,     BT_GET_TIMEOUT_MS},
                          {AT_ROLE,  BT_SET, roleSet, Str_CRLF,           0,    0,     BT_SET_TIMEOUT_MS},
                          {AT_ROLE,  BT_GET, NULL,    Str_CRLF_OK_CRLF,   4,    5,     BT_GET_TIMEOUT_MS},
                          {AT_NAME,  BT_SET, nameSet, Str_CRLF,           0,    0,     BT_SET_TIMEOUT_MS},
                          {AT_NAME,  BT_GET, NULL,    Str_CRLF_OK_CRLF,   4,    5,     BT_GET_TIMEOUT_MS},
                          };

const AtCmdSt_t AtCmdMasterInit[] PROGMEM = {
                          /* CmdIdx,  BtOp,  CmdAddon, TermPattern  MatchLen, SkipLen, TimeoutMs */
                          {AT_AT,    BT_GET, NULL,    Str_CRLF,           0,    0,     BT_GET_TIMEOUT_MS},
                          {AT_RMAAD, BT_SET, NULL,    Str_CRLF,           0,    0,     BT_SET_TIMEOUT_MS},
                          {AT_ROLE,  BT_SET, roleSet, Str_CRLF,           0,    0,     BT_SET_TIMEOUT_MS},
                          {AT_ROLE,  BT_GET, NULL,    Str_CRLF_OK_CRLF,   4,    5,     BT_GET_TIMEOUT_MS},
                          {AT_NAME,  BT_SET, nameSet, Str_CRLF,           0,    0,     BT_SET_TIMEOUT_MS},
                          {AT_NAME,  BT_GET, NULL,    Str_CRLF_OK_CRLF,   4,    5,     BT_GET_TIMEOUT_MS},
                          {AT_INQM,  BT_GET, NULL,    Str_CRLF_OK_CRLF,   4,    5,     BT_GET_TIMEOUT_MS},
                          {AT_INIT,  BT_SET, NULL,    Str_CRLF,           0,    0,     BT_SET_TIMEOUT_MS},
//                          {AT_INQ,   BT_GET, NULL,    Str_CRLF,           3,          20000},
//                          {AT_RNAME, BT_GET, NULL,    Str_CRLF_OK_CRLF,   1,          10000},
                          };

/* PUBLIC FUNTIONS */
/**
 * \file  bluetooth.h
 * \fn void bluetooth_init(HwSerial *hwSerial)
 * \brief Bluetooth initilization
 *
 * \param  hwSerial: pointer on a HwSerial (eg: &Serial1).
 * \return Void.
 */
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
      if(Idx)
      {
        sprintf_P(UartAtCmd, PSTR("AT+UART=%lu,0,0\r\n"), RateTbl[0]);
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
  BT_SEND_AT_SEQ(AtCmdBtInit);
#if (BT_MASTER == 1)
  BT_SEND_AT_SEQ(AtCmdMasterInit);
#else
  BT_SEND_AT_SEQ(AtCmdSlaveInit);
#endif
  AtCmdMode(OFF);
}

/**
 * \file  bluetooth.h
 * \fn int8_t bluetooth_getName(char *RespBuf, uint16_t TimeoutMs)
 * \brief Returns the local bluetooth name
 *
 * \param  RespBuf:   pointer on response buffer (shall be long enough!).
 * \param  TimeoutMs: Timeout in ms.
 * \return < 0: error, > 1, length of the response present in RespBuf.
 */
int8_t bluetooth_getName(char *RespBuf, uint16_t TimeoutMs)
{
  return(setAtCmdAndWaitForResp(AT_NAME, BT_GET, NULL, RespBuf, 4, 5, (char *)"OK\r\n", TimeoutMs));
}

/**
 * \file  bluetooth.h
 * \fn int8_t bluetooth_setName(char *BtName, uint16_t TimeoutMs)
 * \brief sets the local bluetooth name
 *
 * \param  BtName:    Bluetooth local name.
 * \param  TimeoutMs: Timeout in ms.
 * \return < 0: error, >= 0, length of the response present in RespBuf.(normally OK)
 */
int8_t bluetooth_setName(char *BtName, uint16_t TimeoutMs)
{
  char RespBuf[20];

  return(setAtCmdAndWaitForResp(AT_NAME, BT_SET, BtName, RespBuf, 4, 5, (char *)"OK\r\n", TimeoutMs));
}

/**
 * \file  bluetooth.h
 * \fn int8_t bluetooth_getPswd(char *RespBuf, uint16_t TimeoutMs)
 * \brief Returns the local bluetooth Password (PIN)
 *
 * \param  RespBuf:   pointer on response buffer (shall be long enough!).
 * \param  TimeoutMs: Timeout in ms.
 * \return < 0: error, > 1, length of the response present in RespBuf.
 */
int8_t bluetooth_getPswd(char *RespBuf, uint16_t TimeoutMs)
{
  return(setAtCmdAndWaitForResp(AT_PSWD, BT_GET, NULL, RespBuf, 4, 5, (char *)"OK\r\n", TimeoutMs));
}

/**
 * \file  bluetooth.h
 * \fn int8_t bluetooth_setPswd(char *BtName, uint16_t TimeoutMs)
 * \brief sets the local bluetooth password (PIN)
 *
 * \param  BtName:    Bluetooth Password (PIN).
 * \param  TimeoutMs: Timeout in ms.
 * \return < 0: error, >= 0, length of the response present in RespBuf.(normally OK)
 */
int8_t bluetooth_setPswd(char *BtPswd, uint16_t TimeoutMs)
{
  char RespBuf[20];

  return(setAtCmdAndWaitForResp(AT_PSWD, BT_SET, BtPswd, RespBuf, 4, 5, (char *)"OK\r\n", TimeoutMs));
}

/**
 * \file  bluetooth.h
 * \fn int8_t bluetooth_getRemoteName(char *RemoteMacBin, char *RespBuf, uint16_t TimeoutMs)
 * \brief Returns the remote Name
 * \param  RemoteMacBin: pointer on the remote MAC (5 bytes)
 * \param  RespBuf:      pointer on response buffer (shall be long enough!).
 * \param  TimeoutMs:    Timeout in ms.
 * \return < 0: error, > 1, length of the response present in RespBuf.
 */
int8_t bluetooth_getRemoteName(char *RemoteMacBin, char *RespBuf, uint16_t TimeoutMs)
{
  char MacStr[14];
  // Fromat: 25,56,D8CA0F
  return(setAtCmdAndWaitForResp(AT_RNAME, BT_GET, buidMacStr(RemoteMacBin, MacStr), RespBuf, 4, 5, (char *)"\r\nOK\r\n", TimeoutMs));
}


/* PRIVATE FUNCTIONS */
static char *buidMacStr(char *MacBin, char *MacStr)
{
  char    NibbleDigit;
  uint8_t Byte, Idx, Nibble, RemIdx = 0;

  for(Idx = 0; Idx < 5; Idx++)
  {
    Byte = MacBin[Idx];
    Nibble = ((Byte & 0xF0) >> 4);
    NibbleDigit = (Nibble < 10)? '0' + Nibble: 'A' + (Nibble - 10);
    MacStr[RemIdx++] = NibbleDigit;
    Nibble = (Byte & 0x0F);
    NibbleDigit = (Nibble < 10)? '0' + Nibble: 'A' + (Nibble - 10);
    MacStr[RemIdx++] = NibbleDigit;
    if((RemIdx == 2) || (RemIdx == 5)) MacStr[RemIdx++] = ',';
  }
  MacStr[RemIdx] = 0; // En of String

  return(MacStr);
}

static int8_t setAtCmdAndWaitForResp(uint8_t AtCmdIdx, uint8_t BtOp, char *AtCmdArg, char *Resp, uint8_t MatchLen, uint8_t SkipLen, char *TermPattern, uint16_t TimeoutMs)
{
  char     AtCmd[20];
  uint8_t  RxChar, RxIdx = 0;
  uint32_t Start10MsTick, Timeout10msTick;
  uint8_t  Ret = -1;

  while(uCli.stream->available()) uCli.stream->read();

  Resp[0] = 0; /* End of String */
  uCli.stream->print(F("AT"));
  if(AtCmdIdx != AT_AT)
  {
    uCli.stream->print(F("+"));
    uCli.stream->print(getAtCmd(AtCmdIdx, AtCmd));
    if((BtOp != BT_SET) || AtCmdArg) uCli.stream->print((BtOp == BT_GET)? F("?"): F("="));
  }
  if(AtCmdArg)
  {
    uCli.stream->print(AtCmdArg);
  }
  uCli.stream->print(F("\r\n"));
  Start10MsTick = GET_10MS_TICK();
  /* Now, check expected header is received */
  if((AtCmdIdx != AT_AT) && (BtOp == BT_GET))
  {
    /* The response shall start with '+' */
    while((GET_10MS_TICK() - Start10MsTick) < MS_TO_10MS_TICK(TimeoutMs))
    {
      YIELD_TO_TASK(PRIO_TASK_LIST());
      if(uCli.stream->available())
      {
        RxChar = uCli.stream->read();
        if(RxChar != '+') return(Ret);
        else
        {
          break;
        }
      }
    }
  }
  if(MatchLen)
  {
    while((RxIdx < MatchLen) && ((GET_10MS_TICK() - Start10MsTick) < MS_TO_10MS_TICK(TimeoutMs)))
    {
      YIELD_TO_TASK(PRIO_TASK_LIST());
      if(uCli.stream->available())
      {
        RxChar = uCli.stream->read();
        if(RxChar == AtCmd[RxIdx])
        {
          RxIdx++;
        }
        else break;
      }
    }
  }
  /* Now, skip characters if needed */
  if(SkipLen)
  {
    while((RxIdx < SkipLen) && (GET_10MS_TICK() - Start10MsTick) < MS_TO_10MS_TICK(TimeoutMs))
    {
      YIELD_TO_TASK(PRIO_TASK_LIST());
      if(uCli.stream->available())
      {
        RxChar = uCli.stream->read();
        RxIdx++;
      }
    }
  }
  if(RxIdx >= SkipLen)
  {
    /* OK, skipped char received */
    Timeout10msTick = MS_TO_10MS_TICK(TimeoutMs) - (GET_10MS_TICK() - Start10MsTick);
    Ret = getSerialMsg(TermPattern, 1, _10MS_TICK_TO_MS(Timeout10msTick));
  }
  return(Ret);
}

static void AtCmdMode(uint8_t On)
{
  uint32_t StartDurationMs;

//  digitalWrite(BT_KEY_EN, On);
  if(On)
  {
    YIELD_TO_TASK_FOR_MS(PRIO_TASK_LIST(), StartDurationMs, 60);
  }
}

static uint8_t   getAtCmdIdx(const AtCmdSt_t *AtCmdTbl, uint8_t Idx)
{
  return((uint8_t)pgm_read_byte(&AtCmdTbl[Idx].CmdIdx));
}

static uint8_t   getAtBtOp(const AtCmdSt_t *AtCmdTbl, uint8_t Idx)
{
  return((uint8_t)pgm_read_byte(&AtCmdTbl[Idx].BtOp));
}

static char *getAtCmd(uint8_t Idx, char *Buf)
{
  strcpy_P(Buf, (char*)pgm_read_word(&AtCmdTbl[Idx]));
  return(Buf);
}

static uint8_t getAtMatchLen(const AtCmdSt_t *AtCmdTbl, uint8_t Idx)
{
  return((uint8_t)pgm_read_byte(&AtCmdTbl[Idx].MatchLen));
}

static uint8_t getAtSkipLen(const AtCmdSt_t *AtCmdTbl, uint8_t Idx)
{
  return((uint8_t)pgm_read_byte(&AtCmdTbl[Idx].SkipLen));
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

static uint16_t getAtTimeoutMs(const AtCmdSt_t *AtCmdTbl, uint8_t Idx)
{
  return((uint16_t)pgm_read_word(&AtCmdTbl[Idx].TimeoutMs));
}

static int8_t getSerialMsg(char *TermPattern, uint8_t TermPatternNb, uint16_t TimeoutMs)
{
  uint32_t Start10MsTick = GET_10MS_TICK();
  uint8_t  RxChar, TermPatternLen;
  int8_t   Pidx = -1, RxLen = -1;

  TermPatternLen = strlen(TermPattern);
  do
  {
    YIELD_TO_TASK(PRIO_TASK_LIST());
    if(uCli.stream->available() >  0)
    {
      RxChar = uCli.stream->read();
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
            goto AddChar;
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
  }while(((GET_10MS_TICK() - Start10MsTick) < MS_TO_10MS_TICK(TimeoutMs)) && (RxLen < 0));

  return(RxLen);
}

static void btSendAtSeq(const AtCmdSt_t *AtCmdTbl, uint8_t TblItemNb)
{
  uint8_t    Idx, AtCmdIdx, BtOp, MatchLen, SkipLen;
  uint16_t   TimeoutMs;
  AtCmdAddon CmdAddon;
  char       Buf[30];
  char      *AtCmdArg;
  char       TermPattern[10];

  AtCmdMode(ON);
  for(Idx = 0; Idx < TblItemNb; Idx++)
  {
    AtCmdArg  = NULL;
    AtCmdIdx  = getAtCmdIdx(AtCmdTbl,  Idx);
    BtOp      = getAtBtOp(AtCmdTbl,  Idx);
    CmdAddon  = getAtCmdAddon(AtCmdTbl,  Idx);
    if(CmdAddon)
    {
      CmdAddon(Buf);
      AtCmdArg = Buf;
    }
    getAtTermPattern(AtCmdTbl, Idx, TermPattern);
    MatchLen  = getAtMatchLen(AtCmdTbl, Idx);
    SkipLen   = getAtSkipLen(AtCmdTbl, Idx);
    TimeoutMs = getAtTimeoutMs(AtCmdTbl, Idx);
    if(setAtCmdAndWaitForResp(AtCmdIdx, BtOp, AtCmdArg, uCli.CmdLine.Msg, MatchLen, SkipLen, TermPattern, TimeoutMs) >= 0)
    {
//      Serial.print(F("<-'")); Serial.print(uCli.CmdLine.Msg);Serial.println(F("'"));
    }
    else
    {
//      Serial.println(F("No reponse!'"));
    }
  }
  AtCmdMode(OFF);
}

static void uartSet(char* Addon)
{
  strcpy_P(Addon, PSTR("115200,0,0"));
}

static void classSet(char* Addon)
{
  strcpy_P(Addon, PSTR("0"));
}

static void roleSet(char* Addon)
{
  Addon[0] = '0' + BT_MASTER; // To replace with eeprom bit value
  Addon[1] =  0;
}

static void inqmSet(char* Addon)
{
  strcpy(Addon, "0,4,4");
}

static void nameSet(char* Addon)
{
  strcpy(Addon, "RC-NAVY_"); // To replace with eeprom string value
  strcat(Addon, (BT_MASTER == 1)? "M": "S");
}
