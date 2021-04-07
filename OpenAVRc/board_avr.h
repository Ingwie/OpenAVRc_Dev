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


#ifndef board_avr_h
#define board_avr_h

#if defined(PCBMEGA2560)
#include "targets/mega2560/board_mega2560.h"
#elif defined(PCBMEGAMINI)
#include "targets/megamini/board_megamini.h"
#endif

// Port Pin Bitmaps for general use.
// Saves tedious 1<<something. Borrowed from Xmega.
#ifndef PIN0_bm
#define PIN0_bm  0x01
#endif
#ifndef PIN1_bm
#define PIN1_bm  0x02
#endif
#ifndef PIN2_bm
#define PIN2_bm  0x04
#endif
#ifndef PIN3_bm
#define PIN3_bm  0x08
#endif
#ifndef PIN4_bm
#define PIN4_bm  0x10
#endif
#ifndef PIN5_bm
#define PIN5_bm  0x20
#endif
#ifndef PIN6_bm
#define PIN6_bm  0x40
#endif
#ifndef PIN7_bm
#define PIN7_bm  0x80
#endif

// ADC driver
extern void adcInit();
extern void getADC();

// USART driver (static register dispatcher)
#define RXD_DDR1 DDRD
#define RXD_DDR_PIN1 DDD2
#define RXD_PORT1 PORTD
#define RXD_PORT_PIN1 PORTD2
#define RXD_DDR0 DDRE
#define RXD_DDR_PIN0 DDE0
#define RXD_PORT0 PORTE
#define RXD_PORT_PIN0 PORTE0

#define _DOR_N(usart_no) DOR ## usart_no
#define _FE_N(usart_no) FE ## usart_no
#define _RXCIE_N(usart_no) RXCIE ## usart_no
#define _RXC_N(usart_no) RXC ## usart_no
#define _RXD_DDR_N(usart_no) RXD_DDR ## usart_no
#define _RXD_DDR_PIN_N(usart_no) RXD_DDR ## usart_no
#define _RXD_PORT_N(usart_no) RXD_DDR ## usart_no
#define _RXD_PORT_PIN_N(usart_no) RXD_DDR ## usart_no
#define _RXEN_N(usart_no) RXEN ## usart_no
#define _TXCIE_N(usart_no) TXCIE ## usart_no
#define _TXEN_N(usart_no) TXEN ## usart_no
#define _U2X_N(usart_no) U2X ## usart_no
#define _UBRRH_N(usart_no) UBRR ## usart_no ## H
#define _UBRRL_N(usart_no) UBRR ## usart_no ## L
#define _UCSRA_N(usart_no) UCSR ## usart_no ## A
#define _UCSRB_N(usart_no) UCSR ## usart_no ## B
#define _UCSRC_N(usart_no) UCSR ## usart_no ## C
#define _UCSZ0_N(usart_no) UCSZ ## usart_no ## 0
#define _UCSZ1_N(usart_no) UCSZ ## usart_no ## 1
#define _UCSZ2_N(usart_no) UCSZ ## usart_no ## 2
#define _UDRIE_N(usart_no) UDRIE ## usart_no
#define _UDR_N(usart_no) UDR ## usart_no
#define _UPE_N(usart_no) UPE ## usart_no
#define _USART_RX_vect_N(usart_no) USART ## usart_no ## _RX_vect
#define _USART_UDRE_vect_N(usart_no) USART ## usart_no ## _UDRE_vect

#define DOR_N(usart_no) _DOR_N(usart_no)
#define FE_N(usart_no) _FE_N(usart_no)
#define RXCIE_N(usart_no) _RXCIE_N(usart_no)
#define RXC_N(usart_no) _RXC_N(usart_no)
#define RXD_DDR_N(usart_no) _RXD_DDR_N(usart_no)
#define RXD_DDR_PIN_N(usart_no) _RXD_DDR_PIN_N(usart_no)
#define RXD_PORT_N(usart_no) _RXD_PORT_N(usart_no)
#define RXD_PORT_PIN_N(usart_no) _RXD_PORT_PIN_N(usart_no)
#define RXEN_N(usart_no) _RXEN_N(usart_no)
#define TXCIE_N(usart_no) _TXCIE_N(usart_no)
#define TXEN_N(usart_no) _TXEN_N(usart_no)
#define U2X_N(usart_no) _U2X_N(usart_no)
#define UBRRH_N(usart_no) _UBRRH_N(usart_no)
#define UBRRL_N(usart_no) _UBRRL_N(usart_no)
#define UCSRA_N(usart_no) _UCSRA_N(usart_no)
#define UCSRB_N(usart_no) _UCSRB_N(usart_no)
#define UCSRC_N(usart_no) _UCSRC_N(usart_no)
#define UCSZ0_N(usart_no) _UCSZ0_N(usart_no)
#define UCSZ1_N(usart_no) _UCSZ1_N(usart_no)
#define UCSZ2_N(usart_no) _UCSZ2_N(usart_no)
#define UDRIE_N(usart_no) _UDRIE_N(usart_no)
#define UDR_N(usart_no) _UDR_N(usart_no)
#define UPE_N(usart_no) _UPE_N(usart_no)
#define USART_RX_vect_N(usart_no) _USART_RX_vect_N(usart_no)
#define USART_UDRE_vect_N(usart_no) _USART_UDRE_vect_N(usart_no)

#define USART_ENABLE_TX(usartn) \
  { \
    UCSRB_N(usartn) |= (1 << TXEN_N(usartn)); \
  } // Enable TX.

#define USART_PURGE_RX(usartn) { while (UCSRA_N(usartn) & (1 << RXC_N(usartn))) (void) UDR_N(usartn); }

#define USART_ENABLE_RX(usartn) \
  { \
    UCSRB_N(usartn) |= (1 << RXEN_N(usartn)); \
    USART_PURGE_RX(usartn); \
    UCSRB_N(usartn) |= (1 << RXCIE_N(usartn)); \
  } // Enable RX. Flush RX. Enable Interrupt.

#define USART_DISABLE_TX(usartn) \
  { \
    UCSRB_N(usartn) &= ~(1 << UDRIE_N(usartn)); \
    UCSRB_N(usartn) &= ~(1 << TXEN_N(usartn)); \
  } // Disable Interrupt. Disable TX.

#define USART_DISABLE_RX(usartn) \
  { \
    UCSRB_N(usartn) &= ~(1 << RXCIE_N(usartn)); \
    UCSRB_N(usartn) &= ~(1 << RXEN_N(usartn)); \
  } // Disable Interrupt. Disable RX.

#define USART_TRANSMIT_BUFFER(usartn) \
  { \
    UCSRB_N(usartn) |= (1 << UDRIE_N(usartn)); \
  } // Enable Data Register Empty Interrupt to transmit buffer.

#define USART_SET_MODE_8N1(usartn) \
  { \
    UCSRB_N(usartn) = (0 << RXCIE_N(usartn)) | (0 << TXCIE_N(usartn)) | (0 << UDRIE_N(usartn)) | (0 << RXEN_N(usartn)) | (0 << TXEN_N(usartn)) | (0 << UCSZ2_N(usartn)); \
    UCSRC_N(usartn) = 0x06; \
  } // No parity. 1 stop bit.

#define USART_SET_MODE_8E2(usartn) \
  { \
    UCSRB_N(usartn) = (0 << RXCIE_N(usartn)) | (0 << TXCIE_N(usartn)) | (0 << UDRIE_N(usartn)) | (0 << RXEN_N(usartn)) | (0 << TXEN_N(usartn)) | (0 << UCSZ2_N(usartn)); \
    UCSRC_N(usartn) = 0x2E; \
  } // Even parity. 2 stop bits.

#define USART_SET_BAUD(usartn, baud) \
  { \
    UBRRH_N(usartn) = (((F_CPU / (8UL * baud)) - 1UL) >> 8); \
    UBRRL_N(usartn) = (((F_CPU / (8UL * baud)) - 1UL) & 0xFF); \
    UCSRA_N(usartn) |= (1 << U2X_N(usartn)); \
  } // M2560 with 2X mode

#define USART_SET_BAUD_9K6(usartx)    USART_SET_BAUD(usartx, 9600)
#define USART_SET_BAUD_19K2(usartx)   USART_SET_BAUD(usartx, 19200)
#define USART_SET_BAUD_38K4(usartx)   USART_SET_BAUD(usartx, 38400)
#define USART_SET_BAUD_57K6(usartx)   USART_SET_BAUD(usartx, 57600)
#define USART_SET_BAUD_100K(usartx)   USART_SET_BAUD(usartx, 100000)
#define USART_SET_BAUD_115K2(usartx)  USART_SET_BAUD(usartx, 115200)
#define USART_SET_BAUD_125K(usartx)   USART_SET_BAUD(usartx, 125000)


#endif
