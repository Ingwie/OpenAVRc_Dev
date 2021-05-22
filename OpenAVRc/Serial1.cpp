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

#ifdef SIMU
#define SIMUSENDHWSBYTE() \
  while (BT_TX_Fifo.available()) {SendBtSerTxBuffer(BT_TX_Fifo.pop());} SIMU_PROCESSEVENTS;
#else
#define SIMUSENDHWSBYTE()
#endif

#include "Serial1.h"

Fifo<BT_SERIAL_TX_FIFO_SIZE> BT_TX_Fifo;
Fifo<BT_SERIAL_RX_FIFO_SIZE> BT_RX_Fifo;

void BT_Ser_Init(uint8_t speed)
{
 switch (speed)
  {
  case 1:
   USART_SET_BAUD_57K6(TLM_USART1);
  case 2:
   USART_SET_BAUD_38K4(TLM_USART1);
  case 3:
   USART_SET_BAUD_19K2(TLM_USART1);
  case 4:
   USART_SET_BAUD_9K6(TLM_USART1);
  default: // 0
   USART_SET_BAUD_115K2(TLM_USART1);
  }
 USART_SET_MODE_8N1(TLM_USART1);
 USART_ENABLE_TX(TLM_USART1);
 USART_ENABLE_RX(TLM_USART1);
}

void BT_Ser_SendTxBuffer()
{
 SIMUSENDHWSBYTE(); // Send buffer to simu
#if !defined(SIMU)
 if (!(UCSRB_N(TLM_USART1) & _BV(UDRIE_N(TLM_USART1)))) // if we are not in transmit mode
  { // initiate transmition
   UDR_N(TLM_USART1) = BT_TX_Fifo.pop();
   USART_TRANSMIT_BUFFER(TLM_USART1);
  }
#endif
}

void BT_Ser_flushRX()
{
 BT_RX_Fifo.flush();
 USART_PURGE_RX(TLM_USART1); // Flush Rx
}

void BT_Ser_Println()
{
 BT_Ser_Print(Str_CRLF);
 BT_Ser_SendTxBuffer(); // send buffer
}

void BT_Ser_Print(char c)
{
 BT_TX_Fifo.push(c);
}

void BT_Ser_Println(char c)
{
 BT_Ser_Print(c);
 BT_Ser_Println();
}

void BT_Ser_Print(char c[])
{
 uint8_t i = 0;
 while(1)
  {
   char d = c[i++];
   if (d == 0) break;
   BT_TX_Fifo.push(d);
  }
}

void BT_Ser_Println(char c[])
{
 BT_Ser_Print(c);
 BT_Ser_Println();
}

void BT_Ser_Print(PGM_P p)
{
 while(1)
  {
   char c = pgm_read_byte(p++);
   if (c == 0) break;
   BT_TX_Fifo.push(c);
  }
}

void BT_Ser_Println(PGM_P p)
{
 BT_Ser_Print(p);
 BT_Ser_Println();
}

void BT_Ser_Print(const uint8_t * data, uint8_t len)
{
 uint8_t i = 0;
 while(len--)
  {
   BT_TX_Fifo.push(data[i++]);
  }
}

ISR(USART_RX_vect_N(TLM_USART1))
{
// Read new data value
 uint8_t data = UDR_N(TLM_USART1);
// Check error(s) flags
 if (!(UCSRA_N(TLM_USART1) & (_BV(FE_N(TLM_USART0)) | _BV(DOR_N(TLM_USART0)) | _BV(UPE_N(TLM_USART0)))))
  {
   // No error, store data in the buffer if there is room
   BT_RX_Fifo.push(data);
  };
}

// USART1 Transmit Data Register Emtpy ISR (UDR was loaded in Shift Register)
ISR(USART_UDRE_vect_N(TLM_USART1))
{
 if (BT_TX_Fifo.available())
  {
   UDR_N(TLM_USART1) = BT_TX_Fifo.pop();
  }
 else
  {
   UCSRB_N(TLM_USART1) &= ~_BV(UDRIE_N(TLM_USART1)); // Disable UDRE interrupt.
  }
}
