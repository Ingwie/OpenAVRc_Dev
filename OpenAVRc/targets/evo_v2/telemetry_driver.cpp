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


uint8_t TelemetryRxBuffer[NUM_TELEM_RX_BUFFER][TELEM_RX_PACKET_SIZE];

uint8_t * Usart0TxBuffer = pulses2MHz.pbyte; // [USART0_TX_PACKET_SIZE] bytes used

uint8_t Usart0TxBufferCount = 0;



void Usart0EnableTx()
{
  USART_ENABLE_TX(TLM_USART);
  TLM_USART_PORT.DIRSET = USART_TXD_PIN;
}

void Usart0EnableRx()
{
  USART_ENABLE_RX(TLM_USART);
  // Flush RX buffer. Should be flushed already if the receiver was disabled.
  while (TLM_USART.STATUS & USART_RXCIF_bm) (void) TLM_USART.DATA;
  TLM_USART.CTRLA |= USART_RXCINTLVL_MED_gc; // Medium priority.
  TLM_USART_PORT.DIRCLR = USART_RXD_PIN;
}


void Usart0DisableTx()
{
  TLM_USART_PORT.OUTSET = USART_TXD_PIN; // Marking state ... not break condition.

  TLM_USART.CTRLB &= ~USART_TXEN_bm; // Sets port pin to input.
  TLM_USART_PORT.DIRSET = USART_TXD_PIN;
  TLM_USART.CTRLA &= ~USART_DREINTLVL_gm;
  TLM_USART.STATUS &= ~USART_DREIF_bm; // precautionary.
}

void Usart0DisableRx()
{
  TLM_USART.CTRLA &= ~USART_RXCINTLVL_gm;
  // Disabling RX flushes buffer and clears RXCIF.
  TLM_USART.CTRLB &= ~USART_RXEN_bm;
}

void Usart0TransmitBuffer()
{
  TLM_USART.CTRLA |= USART_DREINTLVL_MED_gc; // Medium priority.
}


/*
 * The Xmega USARTs are clocked from the peripheral clock (32MHz).
 * In order to achieve accurate baud rates it uses a fractional baud rate generator.
 *
 * The following table lists the register values to give the Baud rate.
 * It also gives the % error between the actual and specified Baud rate.
 *
 * CLK2X = 0
 * Baud  BSEL  BSCALE Error%
 * 9K6   12    +4     +0.2
 * 19K2  12    +3     +0.2
 * 38K4  12    +2     +0.2
 * 57K6  135   -2     -0.1
 * 76K8  12    +1     +0.2
 * 100K  159   -3
 * 100K  79    -2
 * 115K2 131   -3     -0.1
 * 125K  127   -3
 *
 */


void Usart0Set9600BAUDS() //Frsky "D" telemetry
{
  USART_SET_BAUD_9K6(TLM_USART);
}

void Usart0Set57600BAUDS() //Frsky S.port telemetry
{
  USART_SET_BAUD_57K6(TLM_USART);
}

void Usart0Set100000BAUDS() //Multiprotocole Serial
{
  USART_SET_BAUD_100K(TLM_USART);
}

void Usart0Set125000BAUDS() //DSM Serial protocol
{
  USART_SET_BAUD_125K(TLM_USART);
}

void Usart0Set8N1()
{
  USART_SET_MODE_8N1(TLM_USART);
}

void Usart0Set8E2()
{
  USART_SET_MODE_8E2(TLM_USART);
}


#if defined(FRSKY) || defined(MULTI)
ISR(TLM_USART_RXC_VECT)
{
// ToDo

  uint8_t stat;
  uint8_t data;

//  UCSRB_N(TLM_USART0) &= ~(1 << RXCIE_N(TLM_USART0)); // disable Interrupt

    stat = TLM_USART.STATUS;
    data = TLM_USART.DATA;

    if (stat & (USART_FERR_bm | USART_BUFOVF_bm | USART_PERR_bm) ) {
        // discard buffer and start fresh on any comms error
//        Usart0RxBufferCount = 0;
      }
    else {
//        parseTelemSportByte(data, 0);
      }

//  UCSRB_N(TLM_USART0) |= (1 << RXCIE_N(TLM_USART0)); // enable Interrupt
}
#endif


// USART Transmit Data Register Emtpy ISR (UDR was loaded in Shift Register)
ISR(TLM_USART_DRE_VECT)
{
  if (Usart0TxBufferCount > 0) {
    TLM_USART.DATA = Usart0TxBuffer[--Usart0TxBufferCount];
  }
  else {
    TLM_USART.CTRLA &= ~USART_DREINTLVL_gm;
    TLM_USART.STATUS &= ~USART_DREIF_bm; // precautionary.
  }
}


