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


#ifndef telemetry_driver_h
#define telemetry_driver_h

#define TLM_USART0 0

void Usart0TransmitBuffer();
void Usart0EnableTx();
void Usart0EnableRx();
void Usart0DisableTx();
void Usart0DisableRx();
void Usart0Set8N1();
void Usart0Set8E2();
void Usart0Set9600BAUDS();    //Frsky "D" telemetry
void Usart0Set57600BAUDS();   //Frsky S.port telemetry
void Usart0Set125000BAUDS();  //DSM Serial protocol
void Usart0Set100000BAUDS();  //Multiprotocole Serial

#define USART0_RX_PACKET_SIZE 21 // Modified for FrSky via SPIMODULES.21
#define USART0_TX_PACKET_SIZE 26

extern uint8_t Usart0RxBuffer[USART0_RX_PACKET_SIZE];

extern uint8_t Usart0TxBuffer[USART0_TX_PACKET_SIZE];

extern uint8_t Usart0TxBufferCount;

extern uint8_t Usart0RxBufferCount;

#endif
