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

/*
Note: This HwSerial driver/module by RC-Navy is mainly based on the arduino HardwareSerial.
      This lightweight version allows dimensionning independently (statically) all the Tx/Rx buffers.
      Ingwie : Modded (All is static now) to increase performances (More than X2 speed)
*/

#ifndef BTSERIAL_H
#define BTSERIAL_H

#include "OpenAVRc.h"

#if defined(SIMU)
 #include "targets/simu/simu_interface.h"
#endif

#if defined(CPUM2560)
#define TLM_USART1 1
#endif
#if defined(CPUXMEGA)
#define TLM_USART1 SERIAL1_USART
#define S1_USART 0
#define S1_PORT  D
#define SERIAL1_USART token_paste3(USART, S1_PORT, S1_USART) // e.g. USARTE0
#define SERIAL1_PORT  token_paste2(PORT, S1_PORT) // e.g. PORTE
#define S1_USART_TXD_PIN_CTRL_REG  token_paste3(SERIAL1_PORT.PIN, 3, CTRL) // e.g. "PORTx.PINnCTRL"
#define S1_USART_RXD_PIN_CTRL_REG  token_paste3(SERIAL1_PORT.PIN, 2, CTRL) // e.g. "PORTx.PINnCTRL"
#endif

#if defined(U_CLI)
 #if defined(XMODEM)
  #define S1_TX_FF_SZ                  41u
  #define S1_RX_FF_SZ                  132u//xmodem packet size
 #else
  #define S1_TX_FF_SZ                  41u
  #define S1_RX_FF_SZ                  128u//nedeed by BT +INQ
 #endif
 #define BT_SERIAL_TX_FIFO_SIZE      S1_TX_FF_SZ // Define here the fifo size (SHALL be a power of 2)
 #define BT_SERIAL_RX_FIFO_SIZE      S1_RX_FF_SZ // Define here the fifo size (SHALL be a power of 2)
#endif

extern Fifo<BT_SERIAL_TX_FIFO_SIZE> BT_TX_Fifo;
extern Fifo<BT_SERIAL_RX_FIFO_SIZE> BT_RX_Fifo;

void BT_Ser_flushRX();
void BT_Ser_SendTxBuffer();

void BT_Ser_Init(uint8_t speed);
void BT_Ser_Println();
void BT_Ser_Print(char c);
void BT_Ser_Println(char c);
void BT_Ser_Print(char c[]);
void BT_Ser_Println(char c[]);
void BT_Ser_Print(PGM_P p);
void BT_Ser_Println(PGM_P p);
void BT_Ser_Print(const uint8_t * data, uint8_t len);

#endif
