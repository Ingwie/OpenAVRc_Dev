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

#if defined(FRSKY) || defined(MULTIPROTOCOL)

void telemetryEnableTx(void)
{
  UCSRB_N(TLM_USART) |= (1 << TXEN_N(TLM_USART)); // enable TX
}

void telemetryEnableRx(void)
{
  UCSRB_N(TLM_USART) |= (1 << RXEN_N(TLM_USART));  // enable RX
  UCSRB_N(TLM_USART) |= (1 << RXCIE_N(TLM_USART)); // enable Interrupt
}

void processSerialData(uint8_t data);
extern uint8_t frskyRxBufferCount; // TODO not driver, change name

ISR(USART_RX_vect_N(TLM_USART))
{
  uint8_t stat;
  uint8_t data;

  UCSRB_N(TLM_USART) &= ~(1 << RXCIE_N(TLM_USART)); // disable Interrupt
  sei();

  stat = UCSRA_N(TLM_USART); // USART control and Status Register 0/1 A

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
  // rh = UCSRB_N(TLM_USART); //USART control and Status Register 0/1 B

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

  data = UDR_N(TLM_USART); // USART data register 0

  if (stat & ((1 << FE_N(TLM_USART)) | (1 << DOR_N(TLM_USART)) | (1 << UPE_N(TLM_USART)))) {
    // discard buffer and start fresh on any comms error
    frskyRxBufferCount = 0;
  } else {
    processSerialData(data);
  }

  cli() ;
  UCSRB_N(TLM_USART) |= (1 << RXCIE_N(TLM_USART)); // enable Interrupt
}

void telemetryPortInit()
{
#if !defined(SIMU)
  RXD_DDR_N(TLM_USART) &= ~(1 << RXD_DDR_PIN_N(TLM_USART));   // set RXD pin as input
  RXD_PORT_N(TLM_USART) &= ~(1 << RXD_PORT_PIN_N(TLM_USART)); // disable pullup on RXD pin

#undef BAUD
#if defined(MULTIMODULE)
#define BAUD 100000
#else
#define BAUD 9600
#endif
#include <util/setbaud.h>

  UBRRH_N(TLM_USART) = UBRRH_VALUE;
  UBRRL_N(TLM_USART) = UBRRL_VALUE;
  UCSRA_N(TLM_USART) &= ~(1 << U2X_N(TLM_USART)); // disable double speed operation.

  // set 8N1
  UCSRB_N(TLM_USART) = 0 | (0 << RXCIE_N(TLM_USART)) | (0 << TXCIE_N(TLM_USART)) | (0 << UDRIE_N(TLM_USART)) | (0 << RXEN_N(TLM_USART)) | (0 << TXEN_N(TLM_USART)) | (0 << UCSZ2_N(TLM_USART));
#if defined(MULTIMODULE)
  UCSRC_N(TLM_USART) = 0 | (1 << UPM01) | (1 << USBS0)| (1 << UCSZ1_N(TLM_USART)) | (1 << UCSZ0_N(TLM_USART)); // set 2 stop bits, even parity BIT
#else
  UCSRC_N(TLM_USART) = 0 | (1 << UCSZ1_N(TLM_USART)) | (1 << UCSZ0_N(TLM_USART));
#endif


  while (UCSRA_N(TLM_USART) & (1 << RXC_N(TLM_USART))) UDR_N(TLM_USART); // flush receive buffer

  // These should be running right from power up on a FrSky enabled '9X.
  telemetryEnableTx(); // enable FrSky-Telemetry emission
  frskyTxBufferCount = 0; // TODO not driver code

  telemetryEnableRx(); // enable FrSky-Telemetry reception
#endif
}

void telemetryTransmitBuffer()
{
  UCSRB_N(TLM_USART) |= (1 << UDRIE_N(TLM_USART)); // enable  UDRE1 interrupt
}

#endif
