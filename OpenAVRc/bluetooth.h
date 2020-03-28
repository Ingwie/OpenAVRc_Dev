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

#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "HwSerial.h"
#include "uCli.h"
#include "misclib.h"

#define BT_POWER_ON_OFF_MS         50
#define BT_WAKE_UP_MS              600
#define BT_AT_WAKE_UP_MS           100
#define BT_GET_TIMEOUT_MS          80
#define BT_SET_TIMEOUT_MS          120
#define BT_SCANN_TIMEOUT_MS        8000
#define BT_READ_RNAME_TIMEOUT_MS   1500

extern const char Str_BT_Slave[];
extern const char Str_BT_Master[];

enum {OFF = 0, ON};

/* Code returned by bluetooth_getState(char *RespBuf, uint8_t RespBufMaxLen, uint16_t Timeout) */
enum {BT_UNKNOWN = - 1, BT_INITIALIZED = 0, BT_READY, BT_PAIRABLE, BT_PAIRED, BT_INQUIRING, BT_CONNECTING, BT_CONNECTED, BT_DISCONNECTED};

#define REMOTE_BT_DEV_MAX_NB       3
#define BT_MAC_BIN_LEN             6
#define BT_NAME_STR_LEN            10

typedef struct{
  uint8_t MAC[BT_MAC_BIN_LEN];
  char    Name[BT_NAME_STR_LEN + 1]; // Max Name length is 10 characters (+ 1 for End of String)
}BtRemoteSt_t;

typedef struct{
  BtRemoteSt_t Remote[REMOTE_BT_DEV_MAX_NB];
}BtScannSt_t;

extern const pm_uchar zz_bt[]; // BT glyph

void    bluetooth_init(HwSerial *hwSerial);
void    bluetooth_power(uint8_t On);
void    bluetooth_AtCmdMode(uint8_t On, uint8_t Yield = 1);
int8_t  bluetooth_getState(char *RespBuf, uint8_t RespBufMaxLen, uint16_t TimeoutMs);
int8_t  bluetooth_getName(char *RespBuf, uint8_t RespBufMaxLen, uint16_t TimeoutMs);
int8_t  bluetooth_setName(char *BtName,  uint16_t TimeoutMs);
int8_t  bluetooth_getPswd(char *RespBuf, uint8_t RespBufMaxLen, uint16_t TimeoutMs);
int8_t  bluetooth_setPswd(char *BtPswd,  uint16_t TimeoutMs);
int8_t  bluetooth_getRemoteName(uint8_t *RemoteMacBin, char *RespBuf, uint8_t RespBufMaxLen, uint16_t TimeoutMs);
uint8_t bluetooth_scann(BtScannSt_t *Scann, uint16_t TimeoutMs);
int8_t  bluetooth_linkToRemote(uint8_t *RemoteMacBin, uint16_t TimeoutMs);
void    bluetooth_addSuffix(char* Addon);
void    rebootBT(uint8_t Yield = 1);
void    BT_Wait_Screen();

#endif // BLUETOOTH_H
