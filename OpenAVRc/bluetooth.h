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

#include "misclib.h"
#include "HwSerial.h"
#include "OpenAVRc.h"

/* Code returned by bluetooth_getState(char *RespBuf, uint8_t RespBufMaxLen, uint16_t Timeout) */
enum {BT_UNKNOWN = - 1, BT_INITIALIZED = 0, BT_READY, BT_PAIRABLE, BT_PAIRED, BT_INQUIRING, BT_CONNECTING, BT_CONNECTED, BT_DISCONNECTED};

void   bluetooth_init(HwSerial *hwSerial);
void   bluetooth_power(uint8_t On);
void   bluetooth_reboot(void);
int8_t bluetooth_getState(char *RespBuf, uint8_t RespBufMaxLen, uint16_t Timeout);
int8_t bluetooth_getName(char *RespBuf, uint8_t RespBufMaxLen, uint16_t TimeoutMs);
int8_t bluetooth_setName(char *BtName,  uint16_t TimeoutMs);
int8_t bluetooth_getPswd(char *RespBuf, uint8_t RespBufMaxLen, uint16_t Timeoutms);
int8_t bluetooth_setPswd(char *BtPswd,  uint16_t TimeoutMs);
int8_t bluetooth_getRemoteName(char *RemoteMacBin, char *RespBuf, uint8_t RespBufMaxLen, uint16_t TimeoutMs);

#endif // BLUETOOTH_H
