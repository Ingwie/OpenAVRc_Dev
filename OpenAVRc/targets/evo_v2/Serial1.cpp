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



#if defined(CPUXMEGA)
ISR(token_paste4(USART, S1_PORT, S1_USART, _RXC_vect)) // e.g. USARTE0_RXC_vect
{
  SERIAL0_USART.CTRLA &= ~USART_DREINTLVL_gm; // Disable interrupt.

  uint8_t stat = SERIAL1_USART.STATUS;
  uint8_t data = SERIAL1_USART.DATA;

  if (stat & (USART_FERR_bm | USART_BUFOVF_bm | USART_PERR_bm) )
  {
    // Discard buffer and start fresh on any comm's error
    parseTelemFunction(START_STOP); // reset
  }
  else parseTelemFunction(data);

  SERIAL1_USART.CTRLA |= USART_RXCINTLVL_MED_gc; // Enable medium priority.
}


ISR(token_paste4(USART, S1_PORT, S1_USART, _DRE_vect))
{
  if (Usart0TxBufferCount)
    SERIAL1_USART.DATA = Usart0TxBuffer_p2M[--Usart0TxBufferCount];
  else
    SERIAL1_USART.CTRLA &= ~USART_DREINTLVL_gm;
}
#endif
