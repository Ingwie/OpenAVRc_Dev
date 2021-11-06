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

uint8_t Usart0TxBufferCount = 0;

#if defined(FRSKY) || defined(MULTI)

ISR(USART_RX_vect_N(TLM_USART0))
{
  uint8_t stat;
  uint8_t data;

  UCSRB_N(TLM_USART0) &= ~(1 << RXCIE_N(TLM_USART0)); // disable Interrupt

  NONATOMIC_BLOCK(NONATOMIC_RESTORESTATE)
  {

    stat = UCSRA_N(TLM_USART0); // USART control and Status Register 0/1 A

    /*
                bit      7      6      5      4      3      2      1      0
                        RxC0  TxC0  UDRE0    FE0   DOR0   UPE0   U2X0  MPCM0

                RxC0:   Receive complete
                TXC0:   Transmit Complete
                UDRE0:  USART Data Register Empty
                FE0:    Frame Error
                DOR0:   Data OverRun
                UPE0:   USART Parity Error
                U2X0:   Double Tx Speed
                PCM0:   MultiProcessor Comms Mode
     */
    // rh = UCSRB_N(TLM_USART0); //USART control and Status Register 0/1 B

    /*
              bit      7      6      5      4      3      2      1      0
                   RXCIE0 TxCIE0 UDRIE0  RXEN0  TXEN0 UCSZ02  RXB80  TXB80

              RxCIE0:   Receive Complete int enable
              TXCIE0:   Transmit Complete int enable
              UDRIE0:   USART Data Register Empty int enable
              RXEN0:    Rx Enable
              TXEN0:    Tx Enable
              UCSZ02:   Character Size bit 2
              RXB80:    Rx data bit 8
              TXB80:    Tx data bit 8
    */

    data = UDR_N(TLM_USART0); // USART data register 0

    if (stat & ((1 << FE_N(TLM_USART0)) | (1 << DOR_N(TLM_USART0)) | (1 << UPE_N(TLM_USART0))))
      {
        // discard buffer and start fresh on any comms error
        parseTelemFrskyByte(START_STOP); // reset
      }
    else
      {
        parseTelemFrskyByte(data);
      }

  }
  UCSRB_N(TLM_USART0) |= (1 << RXCIE_N(TLM_USART0)); // enable Interrupt
}
#endif

// USART0 Transmit Data Register Emtpy ISR (UDR was loaded in Shift Register)
ISR(USART_UDRE_vect_N(TLM_USART0))
{
  if (Usart0TxBufferCount) {
    UDR_N(TLM_USART0) = Usart0TxBuffer_p2M[--Usart0TxBufferCount];
  }
  else {
    UCSRB_N(TLM_USART0) &= ~(1 << UDRIE_N(TLM_USART0)); // Disable UDRE interrupt.
  }
}


