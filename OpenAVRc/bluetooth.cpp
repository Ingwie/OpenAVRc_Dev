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


#define PRIO_TASK_LIST()           checkMixer()

#define BT_SEND_AT_SEQ(AtCmdInit)  btSendAtSeq((const AtCmdSt_t*)&AtCmdInit, TBL_ITEM_NB(AtCmdInit))

#define BT_POWER_ON_OFF_MS         50
#define BT_WAKE_UP_MS              600
#define BT_AT_WAKE_UP_MS           50
#define BT_GET_TIMEOUT_MS          60
#define BT_SET_TIMEOUT_MS          100
#define BT_SCANN_TIMEOUT_MS        20000
#define BT_READ_RNAME_TIMEOUT_MS   10000

enum {BT_REBOOT_DATA_MODE = 0, BT_REBOOT_AT_MODE};
enum {BT_GET = 0, BT_SET, BT_CMD};
enum {OFF = 0, ON};

DECL_FLASH_STR2(Str_CRLF,        "\r\n");
DECL_FLASH_STR2(Str_OK_CRLF,     "OK\r\n");
DECL_FLASH_STR2(Str_CRLF_OK_CRLF,"\r\nOK\r\n");

DECL_FLASH_STR2(Str_AT,          "");      // Simple AT command
DECL_FLASH_STR2(Str_STATE,       "STATE"); // BT Status
DECL_FLASH_STR2(Str_PSWD,        "PSWD");  // BT Password (PIN)
DECL_FLASH_STR2(Str_UART,        "UART");  // Uart
DECL_FLASH_STR2(Str_CLASS,       "CLASS"); // Device Class
DECL_FLASH_STR2(Str_RMAAD,       "RMAAD"); // Clears paired list
DECL_FLASH_STR2(Str_ROLE,        "ROLE");  // Role: 0=Slave, 1=Master
DECL_FLASH_STR2(Str_NAME,        "NAME");  // BT module name
DECL_FLASH_STR2(Str_RNAME,       "RNAME"); // Remote BT module name
DECL_FLASH_STR2(Str_CMODE,       "CMODE"); // Inquire - Connection mode
DECL_FLASH_STR2(Str_INIT,        "INIT");  // Initialize the SPP profile lib
DECL_FLASH_STR2(Str_DISC,        "DISC");  // Disconnection
DECL_FLASH_STR2(Str_INQM,        "INQM");  // Inquire Mode
DECL_FLASH_STR2(Str_INQ,         "INQ");   // Inquire Bluetooth device
DECL_FLASH_STR2(Str_INQC,        "INQC");  // Cancel Inquire Bluetooth device
DECL_FLASH_STR2(Str_LINK,        "LINK");  // Link to a specific remote
DECL_FLASH_STR2(Str_RESET,       "RESET"); // Reset

enum {AT_AT = 0, AT_STATE, AT_PSWD, AT_UART, AT_CLASS, AT_RMAAD, AT_ROLE, AT_NAME, AT_RNAME, AT_CMODE, AT_INIT, AT_DISC, AT_INQM, AT_INQ, AT_INQC, AT_LINK, AT_RESET, AT_CMD_MAX_NB};

DECL_FLASH_TBL(AtCmdTbl, char * const) = {Str_AT, Str_STATE, Str_PSWD, Str_UART, Str_CLASS, Str_RMAAD, Str_ROLE, Str_NAME, Str_RNAME, Str_CMODE, Str_INIT, Str_DISC, Str_INQM, Str_INQ, Str_INQC, Str_LINK, Str_RESET};

/* ALL THE STATUS SRINGS THE BT MODULE CAN ANSWER */
DECL_FLASH_STR2(Str_INITIALIZED,  "INITIALIZED");
DECL_FLASH_STR2(Str_READY,        "READY");
DECL_FLASH_STR2(Str_PAIRABLE,     "PAIRABLE");
DECL_FLASH_STR2(Str_PAIRED,       "PAIRED");
DECL_FLASH_STR2(Str_INQUIRING,    "INQUIRING");
DECL_FLASH_STR2(Str_CONNECTING,   "CONNECTING");
DECL_FLASH_STR2(Str_CONNECTED,    "CONNECTED");
DECL_FLASH_STR2(Str_DISCONNECTED, "DISCONNECTED");

DECL_FLASH_TBL(BtStateTbl, char * const) = {Str_INITIALIZED, Str_READY, Str_PAIRABLE, Str_PAIRED, Str_INQUIRING, Str_CONNECTING, Str_CONNECTED, Str_DISCONNECTED};

typedef void (*AtCmdAddon) (char* Addon);

typedef struct{
  uint16_t NAP; // 2 Bytes
  uint8_t  UAP; // 1 Bytes
  uint32_t LAP; // 3 Bytes (here, the last byte is not used)
}MacSt_t;

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
static void    rebootBT(uint8_t Yield = 1);
static uint8_t getAtCmdIdx(const AtCmdSt_t *AtCmdTbl, uint8_t Idx);
static char   *getAtCmd(uint8_t Idx, char *Buf);
static uint8_t getAtMatchLen(const AtCmdSt_t *AtCmdTbl, uint8_t Idx);
static uint8_t getAtSkipLen(const AtCmdSt_t *AtCmdTbl, uint8_t Idx);
static uint8_t getAtBtOp(const AtCmdSt_t *AtCmdTbl, uint8_t Idx);
static int8_t  sendAtCmdAndWaitForResp(uint8_t AtCmdIdx, uint8_t BtOp, char *AtCmdArg, char *RespBuf, uint8_t RespBufMxLen, uint8_t MatchLen, uint8_t SkipLen, char *TermPattern, uint16_t TimeoutMs);
static int8_t  waitForResp(char *RespBuf, uint8_t RespBufMaxLen, char *TermPattern, uint8_t TermPatternNb, uint16_t TimeoutMs);
static void    btSendAtSeq(const AtCmdSt_t *AtCmdTbl, uint8_t TblItemNb);
static char   *buildMacStr(uint8_t *MacBin, char *MacStr);
static uint8_t buildMacBin(char *MacStr, uint8_t *MacBin);

static void    uartSet(char* Addon);
static void    classSet(char* Addon);
static void    roleSet(char* Addon);
static void    nameSet(char* Addon);
static void    inqmSet(char* Addon);

static int8_t  getBtStateIdx(const char *BtState);


DECL_FLASH_TBL(AtCmdBtInit, AtCmdSt_t) = {
                          /* CmdIdx,  BtOp,  CmdAddon, TermPattern  MatchLen, SkipLen, TimeoutMs */
                          {AT_AT,    BT_CMD, NULL,     Str_CRLF,          0,    0,     BT_GET_TIMEOUT_MS},
                          {AT_UART,  BT_SET, uartSet,  Str_CRLF,          0,    0,     BT_SET_TIMEOUT_MS},
                          {AT_CLASS, BT_SET, classSet, Str_CRLF,          0,    0,     BT_SET_TIMEOUT_MS},
                          {AT_INQM,  BT_SET, inqmSet,  Str_CRLF,          0,    0,     BT_SET_TIMEOUT_MS},
                          };

DECL_FLASH_TBL(AtCmdSlaveInit, AtCmdSt_t) = {
                          /* CmdIdx,  BtOp,  CmdAddon, TermPattern  MatchLen, SkipLen, TimeoutMs */
                          {AT_AT,    BT_CMD, NULL,    Str_CRLF,           0,    0,     BT_GET_TIMEOUT_MS},
                          {AT_ROLE,  BT_SET, roleSet, Str_CRLF,           0,    0,     BT_SET_TIMEOUT_MS},
                          {AT_ROLE,  BT_GET, NULL,    Str_CRLF_OK_CRLF,   4,    5,     BT_GET_TIMEOUT_MS},
                          {AT_NAME,  BT_SET, nameSet, Str_CRLF,           0,    0,     BT_SET_TIMEOUT_MS},
                          {AT_NAME,  BT_GET, NULL,    Str_CRLF_OK_CRLF,   4,    5,     BT_GET_TIMEOUT_MS},
                          };

DECL_FLASH_TBL(AtCmdMasterInit, AtCmdSt_t) = {
                          /* CmdIdx,  BtOp,  CmdAddon, TermPattern  MatchLen, SkipLen, TimeoutMs */
                          {AT_AT,    BT_CMD, NULL,    Str_CRLF,           0,    0,     BT_GET_TIMEOUT_MS},
                          //{AT_RMAAD, BT_CMD, NULL,    Str_CRLF,           0,    0,     BT_SET_TIMEOUT_MS},
                          {AT_ROLE,  BT_SET, roleSet, Str_CRLF,           0,    0,     BT_SET_TIMEOUT_MS},
                          {AT_ROLE,  BT_GET, NULL,    Str_CRLF_OK_CRLF,   4,    5,     BT_GET_TIMEOUT_MS},
                          {AT_NAME,  BT_SET, nameSet, Str_CRLF,           0,    0,     BT_SET_TIMEOUT_MS},
                          {AT_NAME,  BT_GET, NULL,    Str_CRLF_OK_CRLF,   4,    5,     BT_GET_TIMEOUT_MS},
                          {AT_INQM,  BT_GET, NULL,    Str_CRLF_OK_CRLF,   4,    5,     BT_GET_TIMEOUT_MS},
                          {AT_INIT,  BT_CMD, NULL,    Str_CRLF,           0,    0,     BT_SET_TIMEOUT_MS},
                          {AT_RESET, BT_CMD, NULL,    Str_CRLF,           0,    0,     BT_SET_TIMEOUT_MS},
                          };

/* PUBLIC FUNTIONS */
/**
 * \file  bluetooth.cpp
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
  char     RespBuf[10];

  rebootBT(0); // Do NOT yield prio tasks here, since they are not initialized yet

  bluetooth_AtCmdMode(ON, 0);
  for(Idx = 0; Idx < TBL_ITEM_NB(RateTbl); Idx++)
  {
    hwSerial->init(RateTbl[Idx]);
    while(hwSerial->available()) hwSerial->read(); // Flush Rx
    hwSerial->print(F("AT\r\n"));
    if((waitForResp(RespBuf, sizeof(RespBuf), (char *)"K\r\n", 1, 100)) > -1)
    {
      /* OK Uart serial rate found */
      if(Idx)
      {
        sprintf_P(UartAtCmd, PSTR("AT+UART=%lu,0,0\r\n"), RateTbl[0]);
        hwSerial->print(UartAtCmd);
        if((waitForResp(RespBuf, sizeof(RespBuf), (char *)"\r\n", 1, 100)) > -1)
        {
          /* Should be OK */
        }
        /* Switch Serial to Rate = 115200 */
        hwSerial->init(RateTbl[0]);
        /* BT Reboot is needed */
        rebootBT(0); // Do NOT yield prio tasks here, since they are not initialized yet
      }
      break;
    }
    else
    {
    }
  }
  bluetooth_AtCmdMode(OFF);
  BT_SEND_AT_SEQ(AtCmdBtInit); // Common to Master and Slave
  if(g_eeGeneral.BT.Master)
  {
    BT_SEND_AT_SEQ(AtCmdMasterInit);
  }
  else
  {
    BT_SEND_AT_SEQ(AtCmdSlaveInit);
  }
}

/**
 * \file  bluetooth.cpp
 * \fn void bluetooth_power(uint8_t On)
 * \brief Switch ON or OFF the BT module
 *
 * \param  On: 0 -> Switch OFF the BT module, 1 -> Switch ON the BT module
 * \return Void.
 */
void bluetooth_power(uint8_t On)
{
  On? BT_POWER_ON() : BT_POWER_OFF();
}

void bluetooth_AtCmdMode(uint8_t On, uint8_t Yield /* = 1*/)
{
  uint32_t StartDurationMs;

  On? BT_KEY_ON() : BT_KEY_OFF();
  if(On)
  {
    if(Yield)
    {
      YIELD_TO_TASK_FOR_MS(PRIO_TASK_LIST(), StartDurationMs, BT_AT_WAKE_UP_MS);
    }
    else _delay_ms(BT_AT_WAKE_UP_MS);
  }
}

/**
 * \file  bluetooth.cpp
 * \fn int8_t bluetooth_getState(char *RespBuf, uint8_t RespBufMaxLen, uint16_t Timeout)
 * \brief Returns the BT module state
 *
 * \param  RespBuf:       pointer on response buffer
 * \param  RespBufMaxLen: maximum length of the response buffer
 * \param  TimeoutMs: Timeout in ms.
 * \return < 0: error, >= 0: the status code defined in bluetooth.h (raw text response in RespBuf)
 */
int8_t bluetooth_getState(char *RespBuf, uint8_t RespBufMaxLen, uint16_t TimeoutMs)
{
  int8_t Ret;
  Ret = sendAtCmdAndWaitForResp(AT_STATE, BT_GET, NULL, RespBuf, RespBufMaxLen, 5, 6, (char *)"\r\nOK\r\n", TimeoutMs);
  if(Ret > 0)
  {
    Ret = getBtStateIdx(RespBuf);
  }
  return(Ret);
}

/**
 * \file  bluetooth.cpp
 * \fn int8_t bluetooth_getName(char *RespBuf, uint16_t TimeoutMs)
 * \brief Returns the local bluetooth name
 *
 * \param  RespBuf:       pointer on response buffer
 * \param  RespBufMaxLen: maximum length of the response buffer
 * \param  TimeoutMs: Timeout in ms.
 * \return < 0: error, > 1, length of the response present in RespBuf.
 */
int8_t bluetooth_getName(char *RespBuf, uint8_t RespBufMaxLen, uint16_t TimeoutMs)
{
  return(sendAtCmdAndWaitForResp(AT_NAME, BT_GET, NULL, RespBuf, RespBufMaxLen, 4, 5, (char *)"OK\r\n", TimeoutMs));
}

/**
 * \file  bluetooth.cpp
 * \fn int8_t bluetooth_setName(char *BtName, uint16_t TimeoutMs)
 * \brief sets the local bluetooth name
 *
 * \param  BtName:    Bluetooth local name.
 * \param  TimeoutMs: Timeout in ms.
 * \return < 0: error, >= 0, length of the response present in RespBuf.(normally OK)
 */
int8_t bluetooth_setName(char *BtName, uint16_t TimeoutMs)
{
  char RespBuf[10];

  return(sendAtCmdAndWaitForResp(AT_NAME, BT_SET, BtName, RespBuf, sizeof(RespBuf), 4, 5, (char *)"OK\r\n", TimeoutMs));
}

/**
 * \file  bluetooth.cpp
 * \fn int8_t bluetooth_getPswd(char *RespBuf, uint16_t TimeoutMs)
 * \brief Returns the local bluetooth Password (PIN)
 *
 * \param  RespBuf:       pointer on response buffer
 * \param  RespBufMaxLen: maximum length of the response buffer
 * \param  TimeoutMs: Timeout in ms.
 * \return < 0: error, > 1, length of the response present in RespBuf.
 */
int8_t bluetooth_getPswd(char *RespBuf, uint8_t RespBufMaxLen, uint16_t TimeoutMs)
{
  return(sendAtCmdAndWaitForResp(AT_PSWD, BT_GET, NULL, RespBuf, RespBufMaxLen, 4, 5, (char *)"OK\r\n", TimeoutMs));
}

/**
 * \file  bluetooth.cpp
 * \fn int8_t bluetooth_setPswd(char *BtName, uint16_t TimeoutMs)
 * \brief sets the local bluetooth password (PIN)
 *
 * \param  BtName:    Bluetooth Password (PIN).
 * \param  TimeoutMs: Timeout in ms.
 * \return < 0: error, >= 0, length of the response present in RespBuf.(normally OK)
 */
int8_t bluetooth_setPswd(char *BtPswd, uint16_t TimeoutMs)
{
  char RespBuf[10];

  return(sendAtCmdAndWaitForResp(AT_PSWD, BT_SET, BtPswd, RespBuf, sizeof(RespBuf), 4, 5, (char *)"OK\r\n", TimeoutMs));
}

/**
 * \file  bluetooth.cpp
 * \fn int8_t bluetooth_getRemoteName(char *RemoteMacBin, char *RespBuf, uint16_t TimeoutMs)
 * \brief Returns the remote Name
 *
 * \param  RemoteMacBin:  pointer on the remote MAC (6 bytes)
 * \param  RespBuf:       pointer on response buffer
 * \param  RespBufMaxLen: maximum length of the response buffer
 * \param  TimeoutMs:    Timeout in ms.
 * \return < 0: error, > 1, length of the response present in RespBuf.
 */
int8_t bluetooth_getRemoteName(uint8_t *RemoteMacBin, char *RespBuf, uint8_t RespBufMaxLen, uint16_t TimeoutMs)
{
  char MacStr[14];
  // Format: [00]25,56,D8CA0F
  return(sendAtCmdAndWaitForResp(AT_RNAME, BT_GET, buildMacStr(RemoteMacBin, MacStr), RespBuf, RespBufMaxLen, 5, 6, (char *)"\r\nOK\r\n", TimeoutMs));
}

int8_t bluetooth_scann(BtScannSt_t *Scann, uint16_t TimeoutMs)
{
  uint32_t StartMs = GET_10MS_TICK();
  char     Buf[1]; // 1 Byte minimum!
  char     RespBuf[40];
  uint8_t  MacBin[BT_MAC_BIN_LEN];
  uint8_t  MacFound =0, AlreadyRegistered;
  int8_t   Ret = -1;

  memset(Scann, 0, sizeof(BtScannSt_t));
  sendAtCmdAndWaitForResp(AT_INQ, BT_CMD, NULL, Buf, sizeof(Buf), 0, 0, (char *)"OK\r\n", 0); // Just send the command without any reception
  do
  {
    RespBuf[0] = 0;
    if((waitForResp(RespBuf, sizeof(RespBuf), (char *)"\r\n", 1, TimeoutMs)) > -1) // Multi-line reception
    {
      if(!memcmp_P(RespBuf, PSTR("+INQ:"), 5))
      {
        if(buildMacBin(RespBuf + 5, MacBin))
        {
          /* Check the MAC is not already in table */
          AlreadyRegistered = 0;
          for(uint8_t Idx = 0; Idx < MacFound; Idx++)
          {
            if(!memcmp(Scann->Remote[Idx].MAC, MacBin, BT_MAC_BIN_LEN))
            {
              AlreadyRegistered = 1;
              break;
            }
          }
          if(!AlreadyRegistered)
          {
            /* Register it! */
            memcpy(Scann->Remote[MacFound].MAC, MacBin, BT_MAC_BIN_LEN);
            MacFound++;
            Ret = MacFound;
            if(MacFound >= REMOTE_BT_DEV_MAX_NB) break;
          }
        }
      }
    }
  }while(((GET_10MS_TICK() - StartMs) < MS_TO_10MS_TICK(TimeoutMs)) && (MacFound < REMOTE_BT_DEV_MAX_NB));
  /* Reboot needed to quit INQ mode */
  rebootBT();
  bluetooth_AtCmdMode(ON); // Switch to AT Mode
  if(MacFound)
  {
    /* Now, get Remote Name(s) */
    for(uint8_t Idx = 0; Idx < MacFound; Idx++)
    {
      for(uint8_t Try = 0; Try < 2; Try++) // Try twice to get more chance to catch it!
      {
        if(bluetooth_getRemoteName(Scann->Remote[Idx].MAC, RespBuf, sizeof(RespBuf), BT_READ_RNAME_TIMEOUT_MS) > 4)
        {
          strncpy(Scann->Remote[Idx].Name, RespBuf, BT_NAME_STR_LEN);
          Scann->Remote[Idx].Name[BT_NAME_STR_LEN - 1] = 0;
          break; // Exit ASAP
        }
      }
    }
  }
  bluetooth_AtCmdMode(OFF);

  return(Ret);
}

/**
 * \file  bluetooth.cpp
 * \fn int8_t bluetooth_linkToRemote(uint8_t *RemoteMacBin, uint16_t TimeoutMs)
 * \brief Links the Master to a specific remote Slave
 *
 * \param  RemoteMacBin:  pointer on the remote MAC (6 bytes)
 * \param  TimeoutMs:    Timeout in ms.
 * \return < 0: error, > 1: OK
 */
int8_t bluetooth_linkToRemote(uint8_t *RemoteMacBin, uint16_t TimeoutMs)
{
  char RespBuf[20];
  char MacStr[20];

  buildMacStr(RemoteMacBin, MacStr);

  return(sendAtCmdAndWaitForResp(AT_LINK, BT_SET, MacStr, RespBuf, sizeof(RespBuf), 4, 5, (char *)"OK\r\n", TimeoutMs));
}

/* PRIVATE FUNCTIONS */

/**
 * \file  bluetooth.cpp
 * \fn    void rebootBT(uint8_t Yield = 1)
 * \brief Reboot the BT module (Needed to take some parameters into account)
 *
 * \param  Yield: 0 -> blocking delay, 1 -> Yield to prio task list (default value)
 * \return Void.
 */
static void rebootBT(uint8_t Yield /* = 1 */)
{
  uint32_t StartDurationMs;

  bluetooth_power(OFF);
  if(Yield)
  {
    YIELD_TO_TASK_FOR_MS(PRIO_TASK_LIST(), StartDurationMs, BT_POWER_ON_OFF_MS);
  }
  else _delay_ms(BT_POWER_ON_OFF_MS);
  bluetooth_AtCmdMode(BT_REBOOT_DATA_MODE); // Set KEY pin to 0
  if(Yield)
  {
    YIELD_TO_TASK_FOR_MS(PRIO_TASK_LIST(), StartDurationMs, BT_POWER_ON_OFF_MS);
  }
  else _delay_ms(BT_POWER_ON_OFF_MS);
  bluetooth_power(ON);
  if(Yield)
  {
    YIELD_TO_TASK_FOR_MS(PRIO_TASK_LIST(), StartDurationMs, BT_WAKE_UP_MS);
  }
  else _delay_ms(BT_WAKE_UP_MS);
}

static uint8_t buildMacBin(char *MacStr, uint8_t *MacBin)
{
  MacSt_t  BT_MAC;
  char    *Field;
  uint8_t  Len, FieldIdx, FieldLen;
  uint32_t Nap, Uap, Lap;
  uint8_t  Ret = 0;
//+INQ:1A:7D:DA7110,1C010C,7FFF -> Should be +INQ:001A:7D:DA7110,1C010C,7FFF
//                                                ^-- Here, the 2 leading zeros are not displayed in AT+INQ response!
  if(*MacStr)
  {
    Len      = strlen(MacStr);
    Field    = MacStr;
    FieldIdx = 0;
    /* Build full MAC since leading 0 of fields may be absent! */
    for(uint8_t Idx = 0; Idx < Len; Idx++)
    {
      if((MacStr[Idx] == ':') || (MacStr[Idx] == ','))
      {
        // Field found
        MacStr[Idx] = 0; // End of String
        FieldLen= strlen(Field);
        if(FieldIdx == 0)
        {
          Nap = strtol(Field, NULL, 16);
          BT_MAC.NAP = HTONS(Nap);
        }else if(FieldIdx == 1)
        {
          Uap = strtol(Field, NULL, 16);
          BT_MAC.UAP = Uap & 0xFF;
        }else if(FieldIdx == 2)
        {
          Lap = strtol(Field, NULL, 16) << 8;
          BT_MAC.LAP = HTONL(Lap);
        }
        if(FieldIdx >= 2) break; // OK finished
        FieldIdx++;
        Field += (FieldLen + 1);
      }
    }
    if(FieldIdx >= 2)
    {
      memcpy((void *)MacBin, (void *)&BT_MAC, BT_MAC_BIN_LEN);
      Ret = 1;
    }
  }

  return(Ret);
}

static char *buildMacStr(uint8_t *MacBin, char *MacStr)
{
  char    NibbleDigit;
  uint8_t Byte, Idx, Nibble, RemIdx = 0;

  for(Idx = 0; Idx < BT_MAC_BIN_LEN; Idx++)
  {
    Byte = MacBin[Idx];
    Nibble = ((Byte & 0xF0) >> 4);
    NibbleDigit = BIN_NBL_TO_HEX_DIGIT(Nibble);
    MacStr[RemIdx++] = NibbleDigit;
    Nibble = (Byte & 0x0F);
    NibbleDigit = BIN_NBL_TO_HEX_DIGIT(Nibble);
    MacStr[RemIdx++] = NibbleDigit;
    if((RemIdx == 4) || (RemIdx == 7)) MacStr[RemIdx++] = ',';
  }
  MacStr[RemIdx] = 0; // End of String

  return(MacStr);
}

static int8_t sendAtCmdAndWaitForResp(uint8_t AtCmdIdx, uint8_t BtOp, char *AtCmdArg, char *RespBuf, uint8_t RespBufMaxLen, uint8_t MatchLen, uint8_t SkipLen, char *TermPattern, uint16_t TimeoutMs)
{
  char     AtCmd[20];
  uint8_t  RxChar, RxIdx = 0;
  uint32_t Start10MsTick, Timeout10msTick;
  int8_t  Ret = -1;

  while(uCli.stream->available()) uCli.stream->read();

  RespBuf[0] = 0; /* End of String */
  uCli.stream->print(F("AT"));
  if(AtCmdIdx != AT_AT)
  {
    uCli.stream->print(F("+"));
    uCli.stream->print(getAtCmd(AtCmdIdx, AtCmd));
    if(BtOp != BT_CMD) uCli.stream->print((BtOp == BT_GET)? F("?"): F("="));
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
  if(TimeoutMs && (RxIdx >= SkipLen))
  {
    /* OK, skipped char received */
    Timeout10msTick = MS_TO_10MS_TICK(TimeoutMs) - (GET_10MS_TICK() - Start10MsTick);
    Ret = waitForResp(RespBuf, RespBufMaxLen, TermPattern, 1, _10MS_TICK_TO_MS(Timeout10msTick));
  }
  return(Ret);
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

static int8_t waitForResp(char *RespBuf, uint8_t RespBufMaxLen, char *TermPattern, uint8_t TermPatternNb, uint16_t TimeoutMs)
{
  uint32_t Start10MsTick = GET_10MS_TICK();
  uint8_t  RxChar, RxIdx = 0, TermPatternLen;
  int8_t   Pidx = -1, RxLen = -1;

  TermPatternLen = strlen(TermPattern);
  do
  {
    YIELD_TO_TASK(PRIO_TASK_LIST());
    if(uCli.stream->available() >  0)
    {
      RxChar = uCli.stream->read();
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
      if(RxIdx < (RespBufMaxLen - 1))
      {
        RespBuf[RxIdx++] = RxChar;
      }
      else
      {
        Pidx = -1; //msg too long!
      }
      if(Pidx >= (TermPatternLen - 1))
      {
        /* Full pattern found -> replace it by End of String */
        TermPatternNb--;
        if(!TermPatternNb)
        {
          RxIdx -= TermPatternLen;
          RespBuf[RxIdx] = 0; // End of String
          RxLen = RxIdx;
        }
      }
    }
  }while(((GET_10MS_TICK() - Start10MsTick) < MS_TO_10MS_TICK(TimeoutMs)) && (RxLen < 0));

  return RxLen;
}

static void btSendAtSeq(const AtCmdSt_t *AtCmdTbl, uint8_t TblItemNb)
{
  uint8_t    Idx, AtCmdIdx, BtOp, MatchLen, SkipLen;
  uint16_t   TimeoutMs;
  AtCmdAddon CmdAddon;
  char       Arg[30];
  char       RespBuf[30];
  char      *AtCmdArg;
  char       TermPattern[10];

  bluetooth_AtCmdMode(ON);
  for(Idx = 0; Idx < TblItemNb; Idx++)
  {
    AtCmdArg  = NULL;
    AtCmdIdx  = getAtCmdIdx(AtCmdTbl,  Idx);
    BtOp      = getAtBtOp(AtCmdTbl,  Idx);
    CmdAddon  = getAtCmdAddon(AtCmdTbl,  Idx);
    if(CmdAddon)
    {
      CmdAddon(Arg);
      AtCmdArg = Arg;
    }
    getAtTermPattern(AtCmdTbl, Idx, TermPattern);
    MatchLen  = getAtMatchLen(AtCmdTbl, Idx);
    SkipLen   = getAtSkipLen(AtCmdTbl, Idx);
    TimeoutMs = getAtTimeoutMs(AtCmdTbl, Idx);
    if(sendAtCmdAndWaitForResp(AtCmdIdx, BtOp, AtCmdArg, RespBuf, sizeof(RespBuf), MatchLen, SkipLen, TermPattern, TimeoutMs) >= 0)
    {
    }
    else
    {
    }
  }
  bluetooth_AtCmdMode(OFF);
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
  Addon[0] = '0' + g_eeGeneral.BT.Master;
  Addon[1] =  0;
}

static void inqmSet(char* Addon)
{
  strcpy_P(Addon, PSTR("0,4,4"));
}

static void nameSet(char* Addon)
{
  char   Name[BT_NAME_STR_LEN + 1];
  int8_t NameLen;

  NameLen = bluetooth_getName(Name, sizeof(Name), 100);
  if(NameLen > 3)
  {
    /* Check if suffix _M or _S is present */
    if((Name[NameLen - 2] == '_') && ((Name[NameLen - 1] == 'M') || (Name[NameLen - 1] == 'S')))
    {
      /* Skip suffix */
      Name[NameLen - 2] = 0;
    }
    strcpy(Addon, Name);
  }
  else
  {
    /* Name absent or too short */
    strcpy_P(Addon, PSTR("HC-05")); // Better than nothing!
  }
  strcat_P(Addon, (g_eeGeneral.BT.Master)? PSTR("_M"): PSTR("_S"));
}

static int8_t getBtStateIdx(const char *BtState)
{
  for(uint8_t Idx = 0; Idx < TBL_ITEM_NB(BtStateTbl); Idx++)
  {
    if(!strcmp_P(BtState, (char*)pgm_read_word(&BtStateTbl[Idx])))
    {
      return(Idx);
    }
  }

  return(-1);
}
