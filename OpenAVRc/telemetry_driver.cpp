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
  UCSRB_N(TLM_USART0) |= (1 << TXEN_N(TLM_USART0)); // enable TX
}

void Usart0EnableRx()
{
  UCSRB_N(TLM_USART0) |= (1 << RXEN_N(TLM_USART0));  // enable RX
  UCSRB_N(TLM_USART0) |= (1 << RXCIE_N(TLM_USART0)); // enable Interrupt
  while (UCSRA_N(TLM_USART0) & (1 << RXC_N(TLM_USART0))) UDR_N(TLM_USART0); // Flush RX buffer.
}

void Usart0DisableTx()
{
  UCSRB_N(TLM_USART0) &= ~(1 << UDRIE_N(TLM_USART0));// disable Interrupt
  UCSRB_N(TLM_USART0) &= ~(1 << TXEN_N(TLM_USART0)); // disable TX
}

void Usart0DisableRx()
{
  UCSRB_N(TLM_USART0) &= ~(1 << RXCIE_N(TLM_USART0)); // disable Interrupt
  UCSRB_N(TLM_USART0) &= ~(1 << RXEN_N(TLM_USART0));  // disable RX
}

void Usart0Set8N1()
{
  UCSRB_N(TLM_USART0) = (0 << RXCIE_N(TLM_USART0)) | (0 << TXCIE_N(TLM_USART0)) | (0 << UDRIE_N(TLM_USART0)) | (0 << RXEN_N(TLM_USART0)) | (0 << TXEN_N(TLM_USART0)) | (0 << UCSZ2_N(TLM_USART0));
  UCSRC_N(TLM_USART0) = (1 << UCSZ1_N(TLM_USART0)) | (1 << UCSZ0_N(TLM_USART0)); // Set 1 stop bit, No parity bit.
}

void Usart0Set8E2()
{
  UCSRB_N(TLM_USART0) = (0 << RXCIE_N(TLM_USART0)) | (0 << TXCIE_N(TLM_USART0)) | (0 << UDRIE_N(TLM_USART0)) | (0 << RXEN_N(TLM_USART0)) | (0 << TXEN_N(TLM_USART0)) | (0 << UCSZ2_N(TLM_USART0));
  UCSRC_N(TLM_USART0) = (1 << UPM01) | (1 << USBS0)| (1 << UCSZ1_N(TLM_USART0)) | (1 << UCSZ0_N(TLM_USART0)); // set 2 stop bits, even parity BIT
}

void Usart0Set9600BAUDS() //Frsky "D" telemetry
{
#undef BAUD
#define BAUD 9600
#if !defined(SIMU)
#include <util/setbaud.h>
  UBRRH_N(TLM_USART0) = UBRRH_VALUE;
  UBRRL_N(TLM_USART0) = UBRRL_VALUE;
#if USE_2X
   UCSRA_N(TLM_USART0) |= (1 << U2X_N(TLM_USART0));
#else
   UCSRA_N(TLM_USART0) &= ~(1 << U2X_N(TLM_USART0));
#endif
#endif
}

void Usart0Set57600BAUDS() //Frsky S.port telemetry
{
#undef BAUD
#define BAUD 57600
#if !defined(SIMU)
#include <util/setbaud.h>
  UBRRH_N(TLM_USART0) = UBRRH_VALUE;
  UBRRL_N(TLM_USART0) = UBRRL_VALUE;
#if USE_2X
   UCSRA_N(TLM_USART0) |= (1 << U2X_N(TLM_USART0));
#else
   UCSRA_N(TLM_USART0) &= ~(1 << U2X_N(TLM_USART0));
#endif
#endif
}

void Usart0Set125000BAUDS() //DSM Serial protocol
{
#undef BAUD
#define BAUD 125000
#if !defined(SIMU)
#include <util/setbaud.h>
  UBRRH_N(TLM_USART0) = UBRRH_VALUE;
  UBRRL_N(TLM_USART0) = UBRRL_VALUE;
#if USE_2X
   UCSRA_N(TLM_USART0) |= (1 << U2X_N(TLM_USART0));
#else
   UCSRA_N(TLM_USART0) &= ~(1 << U2X_N(TLM_USART0));
#endif
#endif
}

void Usart0Set100000BAUDS() //Multiprotocole Serial
{
#undef BAUD
#define BAUD 100000
#if !defined(SIMU)
#include <util/setbaud.h>
  UBRRH_N(TLM_USART0) = UBRRH_VALUE;
  UBRRL_N(TLM_USART0) = UBRRL_VALUE;
#if USE_2X
   UCSRA_N(TLM_USART0) |= (1 << U2X_N(TLM_USART0));
#else
   UCSRA_N(TLM_USART0) &= ~(1 << U2X_N(TLM_USART0));
#endif
#endif
}

void Usart0TransmitBuffer()
{
  UCSRB_N(TLM_USART0) |= (1 << UDRIE_N(TLM_USART0)); // enable Data Register Empty Interrupt
}

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
  if (Usart0TxBufferCount > 0)
    {
      UDR_N(TLM_USART0) = Usart0TxBuffer[--Usart0TxBufferCount];
    }
  else
    {
      UCSRB_N(TLM_USART0) &= ~(1 << UDRIE_N(TLM_USART0)); // Disable UDRE interrupt.
    }
}

#if defined(FRSKY)
void TelemetryValueWithMin::set(uint8_t value)
{
  if (!this->value) {
    this->value = value;
  } else {
        this->value = (((this->value<<1) + value)/3);
        if (this->value<value) { ++this->value; }
  }
  if (!min || value < min) {
    min = value;
  }
}

void TelemetryValueWithMinMax::set(uint8_t value, uint8_t unit)
{
  TelemetryValueWithMin::set(value);
  if (unit != UNIT_VOLTS) {
    this->value = value;
  }
  if (!max || value > max) {
    max = value;
  }
}

void LoadTelemBuffer(uint8_t *data)
{
  for (uint8_t i=0; i<NUM_TELEM_RX_BUFFER; ++i)
  {
    if (TelemetryRxBuffer[i][0] == 0x00) // Check buffer is free
    {
      memcpy(TelemetryRxBuffer[i], data, TELEM_RX_PACKET_SIZE);
      break;
    }
  }
}


#endif

uint16_t getChannelRatio(source_t channel)
{
  return (uint16_t)g_model.telemetry.channels[channel].ratio << g_model.telemetry.channels[channel].multiplier;
}

lcdint_t applyChannelRatio(source_t channel, lcdint_t val)
{
  return ((int32_t)val+g_model.telemetry.channels[channel].offset) * getChannelRatio(channel) * 2 / 51;
}

