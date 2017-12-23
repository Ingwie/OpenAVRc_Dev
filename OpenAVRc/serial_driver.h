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


#ifndef _SERIAL_H_
#define _SERIAL_H_

#include "OpenAVRc.h"


typedef enum serial_tx_state_ {
  TX_STATE_EMPTY = 0, //
  TX_STATE_READY, //
  TX_STATE_BUSY
} serial_tx_state_t;
extern serial_tx_state_t serialTxState;

//! \brief Baudrate selection.
enum SERIAL_BAUDS {
  BAUDS_4800 = 0,
  BAUDS_9600,
  BAUDS_14400,
  BAUDS_19200,
  BAUDS_38400,
  BAUDS_57600,
  BAUDS_76800
};

//! \brief Definition of baudrate settings item choices.
typedef void (*SerialFuncP)(uint8_t event);

extern SerialFuncP RXHandler;

#define MAX_TX_BUFFER 32
//#if 0
extern uint8_t serialTxBuffer[MAX_TX_BUFFER]; // 32 characters
extern uint8_t serialTxBufferCount;
extern uint8_t * ptrTxISR;
//#endif

void SERIAL_Init();
//void SERIAL_transmitBuffer(uint8_t len);
extern void SERIAL_start_uart_send();
extern void SERIAL_end_uart_send();
extern void SERIAL_send_uart_bytes(uint8_t * buf, uint16_t len);

void SERIAL_startTX();
#define IS_TX_BUSY (serialTxState!=TX_STATE_EMPTY)

#endif
