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

#include "OpenAVRc.h"

#ifndef HC06
  #define HC06
#endif

#define BT_SEND_AT_SEQ(AtCmdInit)  btSendAtSeq((const AtCmdSt_t*)&AtCmdInit, TBL_ITEM_NB(AtCmdInit))

enum {BT_REBOOT_DATA_MODE = 0, BT_REBOOT_AT_MODE};
enum {BT_GET = 0, BT_SET, BT_CMD};

DECL_FLASH_STR2(Str_BT_Slave,    "_S");
DECL_FLASH_STR2(Str_BT_Master,   "_M");

DECL_FLASH_STR2(Str_OK_CRLF,     "OK\r\n");
DECL_FLASH_STR2(Str_CRLF_OK_CRLF,"\r\nOK\r\n");
DECL_FLASH_STR2(Str_ATPref,      "AT");    // AT prefix command
DECL_FLASH_STR2(Str_AT,          "");      // Simple AT command
DECL_FLASH_STR2(Str_STATE,       "STATE"); // BT Status
DECL_FLASH_STR2(Str_PSWD,        "PSWD");  // BT Password (PIN)
DECL_FLASH_STR2(Str_UART,        "UART");  // Uart
DECL_FLASH_STR2(Str_CLASS,       "CLASS"); // Device Class
DECL_FLASH_STR2(Str_RMAAD,       "RMAAD"); // Clears paired list

DECL_FLASH_STR2(Str_RNAME,       "RNAME"); // Remote BT module name
DECL_FLASH_STR2(Str_CMODE,       "CMODE"); // Inquire - Connection mode
DECL_FLASH_STR2(Str_INIT,        "INIT");  // Initialize the SPP profile lib
DECL_FLASH_STR2(Str_DISC,        "DISC");  // Disconnection
DECL_FLASH_STR2(Str_INQM,        "INQM");  // Inquire Mode
DECL_FLASH_STR2(Str_INQ,         "INQ");   // Inquire Bluetooth device
DECL_FLASH_STR2(Str_INQC,        "INQC");  // Cancel Inquire Bluetooth device
DECL_FLASH_STR2(Str_LINK,        "LINK");  // Link to a specific remote
DECL_FLASH_STR2(Str_IPSCAN,      "IPSCAN");// Change scan seting
DECL_FLASH_STR2(Str_IAC,         "IAC");   // Set inquire access
DECL_FLASH_STR2(Str_RESET,       "RESET"); // Reset

DECL_FLASH_STR2(Str_ROLE,        "ROLE");  // Role: 0=Slave, 1=Master
DECL_FLASH_STR2(Str_NAME,        "NAME");  // BT module name

#if defined(HC06)
DECL_FLASH_STR2(Str_OK,     "OK");
DECL_FLASH_STR2(Str_BAUD,        "BAUD"); // HC06 Baud
DECL_FLASH_STR2(Str_PO,          "PO"); // HC06 parity Odd
DECL_FLASH_STR2(Str_PE,          "PE"); // HC06 parity Even
DECL_FLASH_STR2(Str_PN,          "PN"); // HC06 parity None
DECL_FLASH_STR2(Str_PIN,         "PIN"); // HC06 PIN code
DECL_FLASH_STR2(Str_VERSION,     "VERSION"); // HC06 Version
#endif

// The following enum and strings must match.
enum {AT_AT = 0, AT_STATE, AT_PSWD, AT_UART, AT_CLASS, AT_RMAAD, AT_ROLE, AT_NAME,
  AT_RNAME, AT_CMODE, AT_INIT, AT_DISC, AT_INQM, AT_INQ, AT_INQC, AT_LINK, AT_IPSCAN, AT_IAC, AT_RESET,
#if defined(HC06)
  AT_BAUD, AT_PO, AT_PE, AT_PN, AT_PIN, AT_VERSION,
#endif
  AT_CMD_MAX_NB};

DECL_FLASH_TBL(AtCmdTbl, char * const) = {Str_AT, Str_STATE, Str_PSWD, Str_UART, Str_CLASS, Str_RMAAD, Str_ROLE, Str_NAME,
  Str_RNAME, Str_CMODE, Str_INIT, Str_DISC, Str_INQM, Str_INQ, Str_INQC, Str_LINK, Str_IPSCAN, Str_IAC, Str_RESET,
#if defined(HC06)
  Str_BAUD, Str_PO, Str_PE, Str_PN, Str_PIN, Str_VERSION,
#endif
  };

/* ALL THE STATUS STRINGS THE BT MODULE CAN ANSWER */
DECL_FLASH_STR2(Str_INITIALIZED, "INITIALIZED");
DECL_FLASH_STR2(Str_READY,       "READY");
DECL_FLASH_STR2(Str_PAIRABLE,    "PAIRABLE");
DECL_FLASH_STR2(Str_PAIRED,      "PAIRED");
DECL_FLASH_STR2(Str_INQUIRING,   "INQUIRING");
DECL_FLASH_STR2(Str_CONNECTING,  "CONNECTING");
DECL_FLASH_STR2(Str_CONNECTED,   "CONNECTED");
DECL_FLASH_STR2(Str_DISCONNECTED,"DISCONNECTED");

#if defined(HC06)
DECL_FLASH_STR2(Str_OK_None,     "OK None"); //
DECL_FLASH_STR2(Str_OK_115200,     "OK115200"); //
DECL_FLASH_STR2(Str_OK_ROLE_S,     "OK+ROLE:S"); //
DECL_FLASH_STR2(Str_OK_SET_PIN,     "OKsetPIN"); //
DECL_FLASH_STR2(Str_OK_SET_NAME,     "OKsetname"); //
#endif

DECL_FLASH_TBL(BtStateTbl, char * const) = {Str_INITIALIZED, Str_READY, Str_PAIRABLE, Str_PAIRED, Str_INQUIRING, Str_CONNECTING, Str_CONNECTED, Str_DISCONNECTED};

typedef void (*AtCmdAddon) (char* Addon);

PACK(typedef struct{
  uint16_t NAP;    // 2 Bytes
  uint32_t UAP:8;  // 1 Bytes
  uint32_t LAP:24; // 3 Bytes (here, the last byte is not used)
})MacSt_t;

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
static uint8_t getAtCmdIdx(const AtCmdSt_t *AtCmdTbl, uint8_t Idx);
static char   *getAtCmd(uint8_t Idx, char *Buf);
static uint8_t getAtMatchLen(const AtCmdSt_t *AtCmdTbl, uint8_t Idx);
static uint8_t getAtSkipLen(const AtCmdSt_t *AtCmdTbl, uint8_t Idx);
static uint8_t getAtBtOp(const AtCmdSt_t *AtCmdTbl, uint8_t Idx);
static int8_t  sendAtCmdAndWaitForResp(uint8_t AtCmdIdx, uint8_t BtOp, char *AtCmdArg, char *RespBuf, uint8_t RespBufMxLen, uint8_t MatchLen, uint8_t SkipLen, const char *TermPattern_P, uint16_t TimeoutMs);
static int8_t  waitForResp(char *RespBuf, uint8_t RespBufMaxLen, const char *TermPattern_P, uint16_t TimeoutMs);
static void    btSendAtSeq(const AtCmdSt_t *AtCmdTbl, uint8_t TblItemNb);
static char   *buildMacStr(uint8_t *MacBin, char *MacStr);
static uint8_t buildMacBin(char *MacStr, uint8_t *MacBin);

static void    baudSet(char* Addon);
static void    uartSet(char* Addon);
static void    classSet(char* Addon);
static void    roleSet(char* Addon);
static void    cmodeSet(char* Addon);
static void    inqmSet(char* Addon);
static void    ipscanSet(char* Addon);
static void    iacSet(char* Addon);

static int8_t  getBtStateIdx(const char *BtState);
static int8_t  clearPairedList(uint16_t TimeoutMs);

#if defined(HC06)
DECL_FLASH_TBL(AtCmdBtInit, AtCmdSt_t) = {
                          /* CmdIdx,    BtOp,  CmdAddon, TermPattern  MatchLen, SkipLen, TimeoutMs */
                          {AT_AT,      BT_CMD, NULL,    Str_OK,          0,    0,     BT_GET_TIMEOUT_MS *4},
                        //{AT_BAUD,    BT_CMD, baudSet, Str_OK_115200 ,  0,    0,     BT_SET_TIMEOUT_MS *3}, // Baud index hex '1' to 'C'
                        //{AT_PO,      BT_CMD, NULL,    Str_AT,          0,    0,     BT_SET_TIMEOUT_MS}, // Parity "Odd"
                        //{AT_PE,      BT_CMD, NULL,    Str_AT,          0,    0,     BT_SET_TIMEOUT_MS}, // Parity "Even"
                          {AT_PN,      BT_CMD, NULL,    Str_OK_None,     0,    0,     BT_SET_TIMEOUT_MS *3}, // Parity "None"
                        //{AT_VERSION, BT_CMD, NULL,    Str_AT,          0,    0,     BT_SET_TIMEOUT_MS}, // My HC06 returns "linvorV1.8"
                          };

DECL_FLASH_TBL(AtCmdSlaveInit, AtCmdSt_t) = {
                          /* CmdIdx,  BtOp,  CmdAddon, TermPattern  MatchLen, SkipLen, TimeoutMs */
                          {AT_AT,    BT_CMD, NULL,    Str_OK,           0,    0,     BT_GET_TIMEOUT_MS *4},
                          {AT_ROLE,  BT_SET, roleSet, Str_OK_ROLE_S,    0,    0,     BT_SET_TIMEOUT_MS *3},
                        //{AT_PIN,   BT_CMD, NULL,    Str_OK_SET_PIN,   0,    0,     BT_SET_TIMEOUT_MS *3}, // 4 digit PIN.
                          };

DECL_FLASH_TBL(AtCmdMasterInit, AtCmdSt_t) = {
                          /* CmdIdx,  BtOp,  CmdAddon, TermPattern  MatchLen, SkipLen, TimeoutMs */
                          {AT_AT,    BT_CMD, NULL,    Str_OK,           0,    0,     BT_GET_TIMEOUT_MS *3},
                        //{AT_ROLE,  BT_SET, roleSet, Str_OK_ROLE_M,    0,    0,     BT_SET_TIMEOUT_MS *3}, // The HC06 alludes to this but probably an error.
                          };
#endif
#if defined(HC05)
DECL_FLASH_TBL(AtCmdBtInit, AtCmdSt_t) = {
                          /* CmdIdx,  BtOp,  CmdAddon, TermPattern  MatchLen, SkipLen, TimeoutMs */
                          {AT_AT,    BT_CMD, NULL,     Str_CRLF,          0,    0,     BT_GET_TIMEOUT_MS},
                          {AT_UART,  BT_SET, uartSet,  Str_CRLF,          0,    0,     BT_SET_TIMEOUT_MS},
                          {AT_CLASS, BT_SET, classSet, Str_CRLF,          0,    0,     BT_SET_TIMEOUT_MS},
                          {AT_INQM,  BT_SET, inqmSet,  Str_CRLF,          0,    0,     BT_SET_TIMEOUT_MS},
                          {AT_IAC,   BT_SET,  iacSet, Str_CRLF,           0,    0,     BT_SET_TIMEOUT_MS},
                          {AT_IPSCAN,BT_SET, ipscanSet,Str_CRLF,          0,    0,     BT_SET_TIMEOUT_MS},
                          };

DECL_FLASH_TBL(AtCmdSlaveInit, AtCmdSt_t) = {
                          /* CmdIdx,  BtOp,  CmdAddon, TermPattern  MatchLen, SkipLen, TimeoutMs */
                          {AT_AT,    BT_CMD, NULL,    Str_CRLF,           0,    0,     BT_GET_TIMEOUT_MS},
                          {AT_ROLE,  BT_SET, roleSet, Str_CRLF,           0,    0,     BT_SET_TIMEOUT_MS},
                          //{AT_ROLE,  BT_GET, NULL,    Str_CRLF_OK_CRLF,   4,    5,     BT_GET_TIMEOUT_MS},
                          //{AT_NAME,  BT_SET, nameSet, Str_CRLF,           0,    0,     BT_SET_TIMEOUT_MS},
                          //{AT_NAME,  BT_GET, NULL,    Str_CRLF_OK_CRLF,   4,    5,     BT_GET_TIMEOUT_MS},
                          //{AT_INIT,  BT_CMD, NULL,    Str_CRLF,           0,    0,     BT_SET_TIMEOUT_MS}, // Ingwie :return error 17 on my BT
                          };

DECL_FLASH_TBL(AtCmdMasterInit, AtCmdSt_t) = {
                          /* CmdIdx,  BtOp,  CmdAddon, TermPattern  MatchLen, SkipLen, TimeoutMs */
                          {AT_AT,    BT_CMD, NULL,    Str_CRLF,           0,    0,     BT_GET_TIMEOUT_MS},
                          //{AT_RMAAD, BT_CMD, NULL,    Str_CRLF,           0,    0,     BT_SET_TIMEOUT_MS},
                          {AT_ROLE,  BT_SET, roleSet, Str_CRLF,           0,    0,     BT_SET_TIMEOUT_MS},
                          {AT_CMODE, BT_SET,cmodeSet, Str_CRLF,           0,    0,     BT_SET_TIMEOUT_MS},
                          //{AT_ROLE,  BT_GET, NULL,    Str_CRLF_OK_CRLF,   4,    5,     BT_GET_TIMEOUT_MS},
                          //{AT_NAME,  BT_SET, nameSet, Str_CRLF,           0,    0,     BT_SET_TIMEOUT_MS},
                          //{AT_NAME,  BT_GET, NULL,    Str_CRLF_OK_CRLF,   4,    5,     BT_GET_TIMEOUT_MS},
                          //{AT_INQM,  BT_GET, NULL,    Str_CRLF_OK_CRLF,   4,    5,     BT_GET_TIMEOUT_MS},
                          {AT_INIT,  BT_CMD, NULL,    Str_CRLF,           0,    0,     BT_SET_TIMEOUT_MS}, // Ingwie :return error 17 on my BT
                          //{AT_RESET, BT_CMD, NULL,    Str_CRLF,           0,    0,     BT_SET_TIMEOUT_MS},
                          };
#endif


/* PUBLIC FUNTIONS */
/**
 * \file  bluetooth.cpp
 * \fn void bluetooth_init(HwSerial *BT_Serial)
 * \brief Bluetooth initilization
 *
 * \param  Void (Use BT_Serial).
 * \return Void.
 */
void bluetooth_init()
{
#if defined(HC06)
  uint32_t RateTbl[] = { 8, 7, 6, 5, 4 }; // HC06 AT+BAUDn Command.
#endif
#if defined(HC05)
  uint32_t RateTbl[] = {115200, 57600, 38400, 19200, 9600};
#endif

  uint8_t Idx;
  char UartAtCmd[30];
  char RespBuf[10];

  if (!g_eeGeneral.BT.Power)
  {
    BT_POWER_OFF();
  }
  else
  {
    BT_Ser_Println(); // After uCli
    BT_Wait_Screen();
    rebootBT(); // EN is ON

    for (Idx = 0; Idx < TBL_ITEM_NB(RateTbl); Idx++)
    {
      BT_Ser_Init(Idx);
      BT_Ser_flushRX();
      if(sendAtCmdAndWaitForResp(AT_AT, BT_CMD, NULL, RespBuf, sizeof(RespBuf), 0, 0, Str_OK, BT_GET_TIMEOUT_MS *4) >= 0)
      {
        /* OK UART serial rate found */
        if (Idx)
        {
          // Set parity first as changing baud stops comm's after response.
          if(sendAtCmdAndWaitForResp(AT_PN, BT_CMD, NULL, RespBuf, sizeof(RespBuf), 0, 0, Str_OK_None, BT_SET_TIMEOUT_MS *3) >= 0);
          else
          {
            g_eeGeneral.BT.Power = 0; // Quit and don't try again.
            return;
          }
          sprintf_P(UartAtCmd, PSTR("AT+BAUD%lu"), RateTbl[0]);
          BT_Ser_Print(UartAtCmd);
          BT_Ser_SendTxBuffer(); // send buffer
          if ((waitForResp(RespBuf, sizeof(RespBuf), Str_OK_115200, BT_SET_TIMEOUT_MS *3)) >= 0) //"OK115200"
          {
            /* Set UART Serial to Rate = 115200 */
            BT_Ser_Init(0);
          }
          else
          {
            g_eeGeneral.BT.Power = 0;
            return;
          }
          /* BT Reboot is needed */ // Why ... maybe HC UART has corruption ?.
          rebootBT();
        }
        break;
      }
    }

//    /* Set UART Serial to Rate = 115200 */ // Not needed.
//    BT_Ser_Init(0);

    BT_SEND_AT_SEQ(AtCmdBtInit); // Common to Master and Slave

#if defined(HC06)
    if(g_eeGeneral.BT.Master)
      g_eeGeneral.BT.Master = 0; // Force slave.
#endif

    if (g_eeGeneral.BT.Master)
    {
      BT_SEND_AT_SEQ(AtCmdMasterInit);
    }
    else
    {
      BT_SEND_AT_SEQ(AtCmdSlaveInit);
    }
  }
  bluetooth_AtCmdMode (OFF);
}


void bluetooth_AtCmdMode(uint8_t On)
{
  uint16_t StartDurationMs;

  BT_Ser_flushRX();
  if(On)
  {
    BT_KEY_ON();
    YIELD_TO_TASK_FOR_MS(PRIO_TASK_LIST(), StartDurationMs, BT_AT_WAKE_UP_MS);
  }
  else
  {
    BT_KEY_OFF();
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
  Ret = sendAtCmdAndWaitForResp(AT_STATE, BT_GET, NULL, RespBuf, RespBufMaxLen, 5, 6, Str_CRLF_OK_CRLF, TimeoutMs);
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
#if defined(HC06)
/*
 *  Maybe consider a fixed name as name cannot be queried with HC06.
  char Name[] = "O_AVRc";

  if(RespBufMaxLen >= strlen(Name))
    strncpy(RespBuf, Name, RespBufMaxLen);
  return (strlen(RespBuf));
*/
  return 0;
#endif
#if defined(HC05)
  return(sendAtCmdAndWaitForResp(AT_NAME, BT_GET, NULL, RespBuf, RespBufMaxLen, 4, 5, Str_CRLF_OK_CRLF, TimeoutMs));
#endif
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
  char Name[BT_NAME_STR_LEN + 3]; // "_S"'\0'

  strcpy(Name, BtName);
  bluetooth_addSuffix(Name);

#if defined(HC06)
  return(sendAtCmdAndWaitForResp(AT_NAME, BT_CMD, Name, RespBuf, sizeof(RespBuf), 4, 5, Str_OK_SET_NAME, TimeoutMs));
#endif
#if defined(HC05)
  return(sendAtCmdAndWaitForResp(AT_NAME, BT_SET, BtName, RespBuf, sizeof(RespBuf), 4, 5, Str_OK_CRLF, TimeoutMs));
#endif
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
#if defined(HC05)
  int8_t Ret;

  Ret = sendAtCmdAndWaitForResp(AT_PSWD, BT_GET, NULL, RespBuf, RespBufMaxLen, 1, 5, Str_CRLF_OK_CRLF, TimeoutMs); // Check only if the answer starts with P (PIN" or PSWD expected !)
  if(Ret > 0)
  {
    if(RespBuf[Ret - 1] == '"')
    {
      RespBuf[Ret - 1] = 0; // Replace last double quote by end of string
      Ret--;
    }
  }
  return(Ret);
#endif
#if defined(HC06)
  // HC06 PIN cannot be queried.
  return 0;
#endif
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
  char CmdBtPswd[7];

#if defined(HC06)
  snprintf_P(CmdBtPswd, 20, PSTR("%s"), BtPswd);
  return(sendAtCmdAndWaitForResp(AT_PIN, BT_CMD, CmdBtPswd, RespBuf, sizeof(RespBuf), 0, 0, Str_OK_SET_PIN, TimeoutMs));
#endif
#if defined(HC05)
  snprintf_P(CmdBtPswd, 20, PSTR("\"%s\""), BtPswd); // Add double quotes
  return(sendAtCmdAndWaitForResp(AT_PSWD, BT_SET, CmdBtPswd, RespBuf, sizeof(RespBuf), 0, 0, Str_OK_CRLF, TimeoutMs));
#endif
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
  char MacStr[15];
  // Format: [00]25,56,D8CA0F

  return(sendAtCmdAndWaitForResp(AT_RNAME, BT_GET, buildMacStr(RemoteMacBin, MacStr), RespBuf, RespBufMaxLen, 5, 6, Str_CRLF_OK_CRLF, TimeoutMs));
}


/**
 * \file  bluetooth.cpp
 * \fn uint8_t bluetooth_scann(BtScannSt_t *Scann, uint16_t TimeoutMs)
 * \brief Scann the Remote BT
 *
 * \param  Scann:        Pointer on a BtScannSt_t structure
 * \param  TimeoutMs:    Timeout in ms.
 * \return The number of Remote BT found (0 to REMOTE_BT_DEV_MAX_NB)
 */
uint8_t bluetooth_scann(BtScannSt_t *Scann, uint16_t TimeoutMs)
{
  uint16_t StartMs = GET_10MS_TICK();
  char     Buf[1]; // 1 Byte minimum!
  char     RespBuf[40];
  uint8_t  MacBin[BT_MAC_BIN_LEN];
  uint8_t  MacFound =0, AlreadyRegistered;
  uint8_t  Ret = 0;

  rebootBT(); // EN is ON
  clearPairedList(BT_SET_TIMEOUT_MS);
  memset(Scann, 0, sizeof(BtScannSt_t));
  sendAtCmdAndWaitForResp(AT_INQ, BT_CMD, NULL, Buf, sizeof(Buf), 0, 0, (char *)"", 0); // Just send the command without any reception
  do
  {
    RespBuf[0] = 0;
    if((waitForResp(RespBuf, sizeof(RespBuf), Str_CRLF, TimeoutMs)) >= 0) // Multi-line reception
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
            lcdDrawSizedTextAtt(0, MacFound*FH, RespBuf, 10, BSS);
            lcdRefresh();
            MacFound++;
            if(MacFound >= REMOTE_BT_DEV_MAX_NB) break;
          }
        }
      }
    }
  }while(((GET_10MS_TICK() - StartMs) < MS_TO_10MS_TICK(TimeoutMs)) && (MacFound < REMOTE_BT_DEV_MAX_NB));

  // Stop INQ ( +INQ :xxxxxxxx are send by other BT modules and continue if we don't ask to stop
  sendAtCmdAndWaitForResp(AT_DISC, BT_CMD, NULL, Buf, sizeof(Buf), 0, 0, Str_OK_CRLF, BT_SET_TIMEOUT_MS); // Ingwie : wait timout HC never return OK on my tests and pairs continue to send +INQXXXXXXXXXX
  rebootBT(); // EN is ON

  if(MacFound)
  {
    /* Now, get Remote Name(s) */
    for(uint8_t Idx = 0; Idx < MacFound; Idx++)
    {
      for(uint8_t Try = 0; Try < 2; Try++) // Try twice to get more chance to catch it!
      {
        if(bluetooth_getRemoteName(Scann->Remote[Idx].MAC, RespBuf, sizeof(RespBuf), BT_READ_RNAME_TIMEOUT_MS) > 0)
        {
#if !defined(SIMU)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#endif
          strncpy(Scann->Remote[Idx].Name, RespBuf, BT_NAME_STR_LEN);
#if !defined(SIMU)
#pragma GCC diagnostic pop
#endif
          Scann->Remote[Idx].Name[BT_NAME_STR_LEN] = 0;
          lcdDrawSizedTextAtt(0,5*FH + Ret*FH, RespBuf, 10, BSS);
          lcdRefresh();
          Ret++; // Mac AND Remote Name found
          break; // Exit ASAP
        }
      }
    }
  }
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
  char RespBuf[10];
  char MacStr[15];

  return(sendAtCmdAndWaitForResp(AT_LINK, BT_SET, buildMacStr(RemoteMacBin, MacStr), RespBuf, sizeof(RespBuf), 0, 0, Str_OK_CRLF, TimeoutMs));
}


/* PRIVATE FUNCTIONS */

/**
 * \file  bluetooth.cpp
 * \fn    void rebootBT()
 * \brief Reboot the BT module (Needed to take some parameters into account)
 *
 * \return Void.
 */
void rebootBT()
{
  uint16_t StartDurationMs;

  bluetooth_AtCmdMode(BT_REBOOT_DATA_MODE); // Set KEY pin to 0
  BT_POWER_OFF();
  YIELD_TO_TASK_FOR_MS(PRIO_TASK_LIST(), StartDurationMs, BT_POWER_ON_OFF_MS);
  BT_POWER_ON();
  YIELD_TO_TASK_FOR_MS(PRIO_TASK_LIST(), StartDurationMs, BT_WAKE_UP_MS);
   // Switch to AT Mode
  bluetooth_AtCmdMode(ON);
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


static int8_t sendAtCmdAndWaitForResp(uint8_t AtCmdIdx, uint8_t BtOp, char *AtCmdArg, char *RespBuf, uint8_t RespBufMaxLen, uint8_t MatchLen, uint8_t SkipLen, const char *TermPattern_P, uint16_t TimeoutMs)
{
  char     AtCmd[20];
  uint8_t  RxChar, RxIdx = 0;
  uint16_t Start10MsTick, Timeout10msTick;
  int8_t  Ret = -1;

  BT_Ser_flushRX();
  RespBuf[0] = 0; /* End of String */
  BT_Ser_Print(Str_ATPref);
  if(AtCmdIdx != AT_AT)
  {
    BT_Ser_Print(PSTR("+"));
    BT_Ser_Print(getAtCmd(AtCmdIdx, AtCmd));
    if(BtOp != BT_CMD) BT_Ser_Print((BtOp == BT_GET)? '?': '=');
  }
  if(AtCmdArg)
  {
    BT_Ser_Print(AtCmdArg);
  }
#if defined(HC06)
  BT_Ser_SendTxBuffer();
#endif
#if defined(HC05)
  BT_Ser_Println();
#endif
  Start10MsTick = GET_10MS_TICK();
  /* Now, check expected header is received */
  if((AtCmdIdx != AT_AT) && (BtOp == BT_GET))
  {
    /* The response shall start with '+' */
    while((GET_10MS_TICK() - Start10MsTick) < MS_TO_10MS_TICK(TimeoutMs))
    {
      YIELD_TO_TASK(PRIO_TASK_LIST());
      if (BT_RX_Fifo.available())
      {
        RxChar = BT_RX_Fifo.pop();
        if(RxChar == '+') break;
      }
    }
  }
  if(MatchLen)
  {
    while((RxIdx < MatchLen) && ((GET_10MS_TICK() - Start10MsTick) < MS_TO_10MS_TICK(TimeoutMs)))
    {
      YIELD_TO_TASK(PRIO_TASK_LIST());
      if (BT_RX_Fifo.available())
      {
        RxChar = BT_RX_Fifo.pop();
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
      if (BT_RX_Fifo.available())
      {
        RxChar = BT_RX_Fifo.pop();
        RxIdx++;
      }
    }
  }
  if(TimeoutMs && (RxIdx >= SkipLen))
  {
    /* OK, skipped char received */
    Timeout10msTick = MS_TO_10MS_TICK(TimeoutMs) - (GET_10MS_TICK() - Start10MsTick);
    Ret = waitForResp(RespBuf, RespBufMaxLen, TermPattern_P, _10MS_TICK_TO_MS(Timeout10msTick));
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

static uint16_t getAtTimeoutMs(const AtCmdSt_t *AtCmdTbl, uint8_t Idx)
{
  return((uint16_t)pgm_read_word(&AtCmdTbl[Idx].TimeoutMs));
}

static int8_t waitForResp(char *RespBuf, uint8_t RespBufMaxLen, const char *TermPattern_P, uint16_t TimeoutMs)
{
  uint16_t Start10MsTick = GET_10MS_TICK();
  uint8_t  RxChar, RxIdx = 0, TPidx = 0, TermPatternLen;
  int8_t   RxLen = -1;
  char     TermPattern[10];

  strcpy_P(TermPattern, TermPattern_P);

  TermPatternLen = strlen(TermPattern);
  do
  {
    YIELD_TO_TASK(PRIO_TASK_LIST());
    if (BT_RX_Fifo.available())
    {
      RxChar = BT_RX_Fifo.pop();
      if(!TPidx)
      {
        /* No match caught yet */
        if(RxChar == TermPattern[TPidx])
        {
          TPidx++;
        }
        else
        {
          if(RxIdx < RespBufMaxLen)
          {
            RespBuf[RxIdx++] = RxChar;
          }
        }
      }
      else
      {
        /* Match in progress */
        if(RxChar == TermPattern[TPidx])
        {
          TPidx++;
        }
        else
        {
          TPidx = 0; /* Match broken */
          if(RxIdx < RespBufMaxLen)
          {
            RespBuf[RxIdx++] = RxChar;
          }
        }
      }

      if(TPidx >= TermPatternLen)
      {
        /* Full pattern found -> replace it by End of String */
        RespBuf[RxIdx] = 0;
        RxLen = RxIdx;
      }
    }
  }while(((GET_10MS_TICK() - Start10MsTick) < MS_TO_10MS_TICK(TimeoutMs)) && (RxLen < 0));

  return RxLen;
}


static void btSendAtCmd(const AtCmdSt_t *AtCmdTbl, uint8_t TblItemNb, uint8_t Idx)
{
  uint8_t    AtCmdIdx, BtOp, MatchLen, SkipLen;
  uint16_t   TimeoutMs;
  AtCmdAddon CmdAddon;
  char       Arg[30];
  char       RespBuf[30];
  char      *AtCmdArg;

  if(Idx < TblItemNb)
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
    MatchLen  = getAtMatchLen(AtCmdTbl, Idx);
    SkipLen   = getAtSkipLen(AtCmdTbl, Idx);
    TimeoutMs = getAtTimeoutMs(AtCmdTbl, Idx);
    if(sendAtCmdAndWaitForResp(AtCmdIdx, BtOp, AtCmdArg, RespBuf, sizeof(RespBuf), MatchLen, SkipLen, (char*)pgm_read_word(&AtCmdTbl[Idx].TermPattern), TimeoutMs) >= 0)
    {
    }
    else
    {
    }
  }
}


static void btSendAtSeq(const AtCmdSt_t *AtCmdTbl, uint8_t TblItemNb)
{
  for (uint8_t Idx = 0; Idx < TblItemNb; Idx++)
  {
    btSendAtCmd(AtCmdTbl, TblItemNb, Idx);
  }
}


#if defined(HC05)
static void uartSet(char* Addon)
{
  strcpy_P(Addon, PSTR("115200,0,0"));
}


static void classSet(char* Addon)
{
  strcpy_P(Addon, PSTR("0"));
}

static void cmodeSet(char* Addon)
{
  Addon[0] = '0'; // CMODE 0 -> Link only with binded pair
  Addon[1] =  0;
}

static void inqmSet(char* Addon)
{
  strcpy_P(Addon, PSTR("0,3,4")); // 4*1.28 = 5.12 seconds
}

static void ipscanSet(char* Addon)
{
  strcpy_P(Addon, PSTR("1024,1,1024,1"));
}

static void iacSet(char* Addon)
{
  strcpy_P(Addon, PSTR("0x9E8B33"));
}
#endif

static void roleSet(char* Addon)
{
#if defined(HC06)
if(g_eeGeneral.BT.Master)
  strcpy_P(Addon, PSTR("M"));
else
  strcpy_P(Addon, PSTR("S"));
#else
// HC05
  Addon[0] = '0' + g_eeGeneral.BT.Master;
  Addon[1] =  0;
#endif
}

#if defined(HC06)
static void baudSet(char* Addon)
{
  strcpy_P(Addon, PSTR("8")); // HC06 115200
}
#endif


void bluetooth_addSuffix(char* Addon)
{
  strcat_P(Addon, (g_eeGeneral.BT.Master)? Str_BT_Master: Str_BT_Slave);
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


static int8_t clearPairedList(uint16_t TimeoutMs)
{
  char RespBuf[20];
  return(sendAtCmdAndWaitForResp(AT_RMAAD, BT_CMD, NULL, RespBuf, sizeof(RespBuf), 0, 0, Str_OK_CRLF, TimeoutMs));
}

void BT_Send_Channels()
{
 char txt;
 uint8_t ComputedCheckSum = 0;

 BT_Ser_Print(PSTR("tf "));

 for(uint8_t Idx = 0; Idx < NUM_TRAINER; Idx++)
  {
   BT_Ser_Print('s');
   int16_t value = (FULL_CHANNEL_OUTPUTS(Idx))/2; // +-1280 to +-640
   value += PPM_CENTER; // + 1500 offset
   ComputedCheckSum ^= 's';
   value <<= 4;
   for(uint8_t j = 12; j ; j-=4)
    {
     txt = BIN_NBL_TO_HEX_DIGIT((value>>j) & 0x0F);
     ComputedCheckSum ^= txt;
     BT_Ser_Print(txt);
    }
  }
 BT_Ser_Print(':');
 txt = BIN_NBL_TO_HEX_DIGIT(ComputedCheckSum>>4 & 0x0F);
 BT_Ser_Print(txt);
 txt = BIN_NBL_TO_HEX_DIGIT(ComputedCheckSum & 0x0F);
 BT_Ser_Println(txt);
}

const pm_uchar zz_bt[] PROGMEM =
{
#if defined (LCDROT180)
#include "bitmaps/bt.lbmi"
#else
#include "bitmaps/bt.lbm"
#endif
};

void BT_Wait_Screen()
{
 lcdClear();
 lcd_imgfar(10*FW, 3*FH, (pgm_get_far_address(zz_bt)), 0, 0);
 lcdRefresh();
}
