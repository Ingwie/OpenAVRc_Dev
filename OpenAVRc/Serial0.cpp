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

uint8_t Usart0TxBufferCount = 0;

NOINLINE const void parseSerialTelemFakeByte(uint8_t data, uint8_t error)
{
  data = error; // compiler pleased
}

#if defined(CPUM2560)
ISR(USART_RX_vect_N(TLM_USART0))
{
  uint8_t data = UDR_N(TLM_USART0);
  uint8_t error = UCSRA_N(TLM_USART0);
  // Filter FE0:Frame Error DOR0:Data OverRun UPE0:Parity Error
  error &= ((1 << FE_N(TLM_USART0)) | (1 << DOR_N(TLM_USART0)) | (1 << UPE_N(TLM_USART0)));
  parseSerialTelemFunction(data, error);
}


// USART0 Transmit Data Register Emtpy ISR (UDR was loaded in Shift Register)
ISR(USART_UDRE_vect_N(TLM_USART0))
{
  if (Usart0TxBufferCount)
    {
      UDR_N(TLM_USART0) = Usart0TxBuffer_p2M[--Usart0TxBufferCount];
    }
  else
    {
      UCSRB_N(TLM_USART0) &= ~(1 << UDRIE_N(TLM_USART0)); // Disable UDRE interrupt.
    }
}
#endif

#if defined(CPUXMEGA)
ISR(token_paste4(USART, S0_PORT, S0_USART, _RXC_vect)) // e.g. USARTE0_RXC_vect
{
  uint8_t error = SERIAL0_USART.STATUS;
  uint8_t data = SERIAL0_USART.DATA;
  // Filter usart error
  error &= (USART_FERR_bm | USART_BUFOVF_bm | USART_PERR_bm);

  parseSerialTelemFunction(data, error);
}


ISR(token_paste4(USART, S0_PORT, S0_USART, _DRE_vect))
{
  if (Usart0TxBufferCount)
    SERIAL0_USART.DATA = Usart0TxBuffer_p2M[--Usart0TxBufferCount];
  else
  {
    SERIAL0_USART.CTRLA &= ~USART_DREINTLVL_gm;

#if (SERIAL_PROTOCOL == CRSF)
// Half Duplex (Inverted) Serial.
// WAIT_USART_TX_FIN(CRSF_USART); // Wait for transmit complete.
  CRSF_USART.CTRLA |= USART_TXCINTLVL_MED_gc; // Turn on TXC interrupt rather than poll TXC.
#endif
  }
}

#if (SERIAL_PROTOCOL == CRSF)
ISR(token_paste4(USART, S0_PORT, S0_USART, _TXC_vect))
{ // Used for CRSF Half Duplex Serial.
  CRSF_USART.CTRLA &= ~USART_TXCINTLVL_gm; // Turn off TXC interrupts.
  RF_PORT.DIRCLR = USART_TXD_PIN_bm; // Make TXD pin input.
  USART_ENABLE_RX(CRSF_USART); // Enable receiver.
  CRSF_RX_STATE = LISTEN; // Tell telemetry state machine to start listening.
}
#endif
#endif

